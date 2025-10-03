#include "ffmpeg_waveform.h"
#include <QDebug>
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
    
    // Set resampler parameters - 使用兼容性更好的API
    AVChannelLayout in_ch_layout = AV_CHANNEL_LAYOUT_STEREO;
    AVChannelLayout out_ch_layout = AV_CHANNEL_LAYOUT_MONO;
    
    // 如果原始音频只有一个声道，使用单声道布局
    if (codecpar->ch_layout.nb_channels == 1) {
        in_ch_layout = AV_CHANNEL_LAYOUT_MONO;
    }
    
    av_opt_set_chlayout(swr_ctx, "in_chlayout", &in_ch_layout, 0);
    av_opt_set_chlayout(swr_ctx, "out_chlayout", &out_ch_layout, 0);
    av_opt_set_int(swr_ctx, "in_sample_rate", codecpar->sample_rate, 0);
    av_opt_set_int(swr_ctx, "out_sample_rate", codecpar->sample_rate, 0);
    av_opt_set_sample_fmt(swr_ctx, "in_sample_fmt", static_cast<AVSampleFormat>(codecpar->format), 0);
    av_opt_set_sample_fmt(swr_ctx, "out_sample_fmt", AV_SAMPLE_FMT_FLT, 0);
    
    if (swr_init(swr_ctx) < 0) {
        qWarning() << "Failed to initialize resampler";
        swr_free(&swr_ctx);
        avcodec_free_context(&codec_ctx);
        avformat_close_input(&fmt_ctx);
        return waveform;
    }
    
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
                    // Resample
                    uint8_t** out_data = nullptr;
                    int out_samples = av_rescale_rnd(swr_get_delay(swr_ctx, codecpar->sample_rate) + 
                                                   frame->nb_samples, codecpar->sample_rate, codecpar->sample_rate, AV_ROUND_UP);
                    
                    if (av_samples_alloc_array_and_samples(&out_data, nullptr, 1, out_samples, AV_SAMPLE_FMT_FLT, 0) < 0) {
                        qWarning() << "Failed to allocate samples";
                        av_frame_unref(frame);
                        continue;
                    }
                    
                    // Perform resampling
                    int converted = swr_convert(swr_ctx, out_data, out_samples,
                                               (const uint8_t**)frame->data, frame->nb_samples);
                    
                    if (converted > 0) {
                        float* out = reinterpret_cast<float*>(out_data[0]);
                        // Collect samples
                        for (int i = 0; i < converted; i++) {
                            samples.append(out[i]);
                        }
                    }
                    
                    // Free resample buffer
                    if (out_data) {
                        av_freep(&out_data[0]);
                        av_freep(&out_data);
                    }
                }
            }
        }
        av_packet_unref(pkt);
    }
    
cleanup:
    // Clean up resources
    if (frame) av_frame_free(&frame);
    if (pkt) av_packet_free(&pkt);
    if (swr_ctx) swr_free(&swr_ctx);
    if (codec_ctx) avcodec_free_context(&codec_ctx);
    if (fmt_ctx) avformat_close_input(&fmt_ctx);
    
    // Generate waveform data
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
        // If not enough samples, use all samples directly
        waveform = samples;
    }
    
    return waveform;
}
