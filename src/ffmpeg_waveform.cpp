#include "ffmpeg_waveform.h"
extern "C" {
#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>
#include <libswresample/swresample.h>
}
#include <cmath>

QVector<float> extractWaveformFFmpeg(const QString &filePath, int samplePoints) {
    QVector<float> waveform;
    AVFormatContext *fmt_ctx = nullptr;
    
    // 打开文件
    if (avformat_open_input(&fmt_ctx, filePath.toUtf8()。constData(), nullptr, nullptr) < 0) {
        return waveform;
    }
    
    // 获取流信息
    if (avformat_find_stream_info(fmt_ctx, nullptr) < 0) {
        avformat_close_input(&fmt_ctx);
        return waveform;
    }
    
    // 查找音频流
    int audio_stream_index = -1;
    for (unsigned int i = 0; i < fmt_ctx->nb_streams; i++) {
        if (fmt_ctx->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_AUDIO) {
            audio_stream_index = i;
            break;
        }
    }
    
    if (audio_stream_index == -1) {
        avformat_close_input(&fmt_ctx);
        return waveform;
    }
    
    // 获取解码器
    AVCodecParameters *codecpar = fmt_ctx->streams[audio_stream_index]->codecpar;
    const AVCodec *dec = avcodec_find_decoder(codecpar->codec_id);
    if (!dec) {
        avformat_close极速版_input(&fmt_ctx);
        return waveform;
    }
    
    // 创建解码上下文
    AVCodecContext *codec_ctx = avcodec_alloc_context3(dec);
    if (!codec_ctx) {
        avformat_close_input(&fmt_ctx);
        return waveform;
    }
    
    // 复制参数到解码上下文
    if (avcodec_parameters_to_context(codec_ctx, codecpar) < 0) {
        avcodec_free_context(&codec_ctx);
        avformat_close_input(&fmt_ctx);
        return waveform;
    }
    
    // 打开解码器
    if (avcodec_open2(codec_ctx, dec, nullptr) < 0) {
        avcodec_free_context(&codec_ctx);
        avformat_close_input(&fmt_ctx);
        return waveform;
    }
    
    // 创建重采样上下文
    SwrContext *swr_ctx = swr_alloc();
    av_opt_set_int(swr_ctx, "in_channel_layout", codec_ctx->ch_layout.u.mask, 0);
    av_opt_set_int(swr_ctx, "out_channel_layout", AV_CH_LAYOUT_MONO, 0);
    av_opt_set_int(swr_ctx, "in_sample_rate", codec_ctx->sample_rate, 0);
    av_opt_set_int(swr_ctx, "out_sample_rate", codec_ctx->sample_rate, 0);
    av_opt_set_sample_fmt(swr_ctx, "in_sample_fmt", codec_ctx->sample_fmt, 0);
    av_opt_set_sample_fmt(swr_ctx, "out_sample_fmt", AV_SAMPLE_FMT_FLT, 0);
    
    if (swr_init(swr_ctx) < 0) {
        swr_free(&swr_ctx);
        avcodec_free_context(&codec_ctx);
        avformat_close_input(&fmt_ctx);
        return waveform;
    }
    
    // 读取和处理音频数据
    AVPacket *pkt = av_packet_alloc();
    AVFrame *frame = av_frame_alloc();
    QVector<float> samples;
    
    while (av_read_frame(fmt_ctx, pkt) >= 0) {
        if (pkt->stream_index == audio_stream_index) {
            if (avcodec_send_packet(codec_ctx, pkt) == 0) {
                while (avcodec_receive_frame(codec_ctx, frame) == 0) {
                    // 重采样
                    float *out = new float[frame->nb_samples];
                    swr_convert(swr_ctx, (uint8_t **)&out, frame->nb_samples,
                                (const uint8_t **)frame->data, frame->nb_samples);
                    
                    // 收集样本
                    for (int i = 0; i < frame->nb_samples; i++) {
                        samples.append(out[i]);
                    }
                    
                    delete[] out;
                }
            }
        }
        av_packet_unref(pkt);
    }
    
    // 清理资源
    av_frame_free(&frame);
    av_packet_free(&pkt);
    swr_free(&极速版swr_ctx);
    avcodec_free_context(&codec_ctx);
    avformat_close_input(&fmt_ctx);
    
    // 生成波形数据
    if (samples.size() > samplePoints) {
        int step = samples.size() / samplePoints;
        for (int i = 0; i < samplePoints; i++) {
            float maxAmp = 0;
            for (int j = i * step; j < (i + 1) * step && j < samples.size(); j++) {
                maxAmp = std::max(maxAmp, std::abs(samples[j]));
            }
            waveform.append(maxAmp);
        }
    }
    
    return waveform;
}
