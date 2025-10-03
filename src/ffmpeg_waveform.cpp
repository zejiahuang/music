#include "ffmpeg_waveform.h"
extern "C" {
#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>
#include <libswresample/swresample.h>
#include <libavutil/opt.h>
#include <libavutil/channel_layout.h>
}
#include <cmath>

QVector<float> extractWaveformFFmpeg(const QString& filePath, int samplePoints) {
    QVector<float> waveform;
    AVFormatContext* fmt_ctx = nullptr;
    
    // Open file
    if (avformat_open_input(&fmt_ctx, filePath.toUtf8().constData(), nullptr, nullptr) < 0) {
        qWarning() << "Failed to open file:" << filePath;
        return waveform;
    }
    
    // Get stream info
    if (avformat_find_stream_info(fmt_ctx, nullptr) < 0) {
        qWarning() << "Failed to find stream info";
        avformat_close_input(&fmt_ctx);
        return waveform;
    }
    
    // Find audio stream
    int audio_stream_index = -1;
    for (unsigned int i = 0; i < fmt_ctx->nb_streams; i++) {
        if (fmt_ctx->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_AUDIO) {
            audio_stream_index = i;
            break;
        }
    }
    
    if (audio_stream_index == -1) {
        qWarning() << "No audio stream found";
        avformat_close_input(&fmt_ctx);
        return waveform;
    }
    
    // Get decoder
    AVCodecParameters* codecpar = fmt_ctx->streams[audio_stream_index]->codecpar;
    const AVCodec* dec = avcodec_find_decoder(codecpar->codec_id);
    if (!dec) {
        qWarning() << "Unsupported codec:" << codecpar->codec_id;
        avformat_close_input(&fmt_ctx);
        return waveform;
    }
    
    // Create decoder context
    AVCodecContext* codec_ctx = avcodec_alloc_context3(dec);
    if (!codec_ctx) {
        qWarning() << "Failed to allocate codec context";
        avformat_close_input(&fmt_ctx);
        return waveform;
    }
    
    // Copy parameters to decoder context
    if (avcodec_parameters_to_context(codec_ctx, codecpar) < 0) {
        qWarning() << "Failed to copy codec parameters";
        avcodec_free_context(&codec_ctx);
        avformat_close_input(&fmt_ctx);
        return waveform;
    }
    
    // Open decoder
    if (avcodec_open2(codec_ctx, dec, nullptr) < 0) {
        qWarning() << "Failed to open codec";
        avcodec_free_context(&codec_ctx);
        avformat_close_input(&fmt_ctx);
        return waveform;
    }
    
    // Create resampler context
    SwrContext* swr_ctx = swr_alloc();
    if (!swr_ctx) {
        qWarning() << "Failed to allocate resampler";
        avcodec_free_context(&codec_ctx);
        avformat_close_input(&fmt_ctx);
        return waveform;
    }
    
    // Set resampler parameters - UPDATED for newer FFmpeg API
    // 处理输入通道布局
    AVChannelLayout in_ch_layout = codecpar->ch_layout;
    AVChannelLayout out_ch_layout;
    av_channel_layout_default(&out_ch_layout, 1); // 单声道
    
    // 设置重采样参数
    if (swr_alloc_set_opts2(&swr_ctx,
                           &out_ch_layout, AV_SAMPLE_FMT_FLT, codecpar->sample_rate,
                           &in_ch_layout, static_cast<AVSampleFormat>(codecpar->format), codecpar->sample_rate,
                           0, nullptr) < 0) {
        qWarning() << "Failed to set resampler options";
        swr_free(&swr_ctx);
        avcodec_free_context(&codec_ctx);
        avformat_close_input(&fmt_ctx);
        av_channel_layout_uninit(&out_ch_layout);
        return waveform;
    }
    
    if (swr_init(swr_ctx) < 0) {
        qWarning() << "Failed to initialize resampler";
        swr_free(&swr_ctx);
        avcodec_free_context(&codec_ctx);
        avformat_close_input(&fmt_ctx);
        av_channel_layout_uninit(&out_ch_layout);
        return waveform;
    }
    
    // 释放通道布局内存
    av_channel_layout_uninit(&out_ch_layout);
    
    // Read and process audio data
    AVPacket* pkt = av_packet_alloc();
    AVFrame* frame = av_frame_alloc();
    QVector<float> samples;
    
    if (!pkt || !frame) {
        qWarning() << "Failed to allocate packet or frame";
        goto cleanup;
    }
    
    while (av_read_frame(fmt_ctx, pkt) >= 0) {
        if (pkt->stream_index == audio_stream_index) {
            if (avcodec_send_packet(codec_ctx, pkt) == 0) {
                while (avcodec_receive_frame(codec_ctx, frame) == 0) {
                    // 计算输出样本数 - UPDATED API
                    int out_samples = av_rescale_rnd(swr_get_delay(swr_ctx, codecpar->sample_rate) + 
                                                   frame->nb_samples, codecpar->sample_rate, codecpar->sample_rate, AV_ROUND_UP);
                    
                    // 分配输出缓冲区 - UPDATED API
                    uint8_t* out_data = nullptr;
                    int linesize;
                    if (av_samples_alloc(&out_data, &linesize, 1, out_samples, AV_SAMPLE_FMT_FLT, 0) < 0) {
                        qWarning() << "Failed to allocate samples";
                        av_frame_unref(frame);
                        continue;
                    }
                    
                    // 执行重采样 - UPDATED API
                    int converted = swr_convert(swr_ctx, &out_data, out_samples,
                                               (const uint8_t**)frame->data, frame->nb_samples);
                    
                    if (converted > 0) {
                        float* out = reinterpret_cast<float*>(out_data);
                        // 收集样本
                        for (int i = 0; i < converted; i++) {
                            samples.append(out[i]);
                        }
                    }
                    
                    // 释放重采样缓冲区
                    av_freep(&out_data);
                }
            }
        }
        av_packet_unref(pkt);
    }
    
    // 生成波形数据
    if (samples.size() > samplePoints) {
        int step = samples.size() / samplePoints;
        for (int i = 0; i < samplePoints; i++) {
            float maxAmp = 0;
            int start = i * step;
            int end = std::min((i + 1) * step, samples.size());
            for (int j = start; j < end; j++) {
                maxAmp = std::max(maxAmp, std::abs(samples[j]));
            }
            waveform.append(maxAmp);
        }
    } else {
        // 如果样本数不足，直接使用所有样本
        waveform = samples;
    }
    
cleanup:
    // Clean up resources
    if (frame) av_frame_free(&frame);
    if (pkt) av_packet_free(&pkt);
    if (swr_ctx) swr_free(&swr_ctx);
    if (codec_ctx) avcodec_free_context(&codec_ctx);
    if (fmt_ctx) avformat_close_input(&fmt_ctx);
    
    return waveform;
}
