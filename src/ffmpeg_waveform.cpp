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
    if (avformat_open_input(&fmt_ctx, filePath.toStdString().c_str(), nullptr, nullptr) < 0)
        return waveform;
    if (avformat_find_stream_info(fmt_ctx, nullptr) < 0)
        return waveform;
    int audio_stream_index = -1;
    for (unsigned i = 0; i < fmt_ctx->nb_streams; ++i) {
        if (fmt_ctx->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_AUDIO) {
            audio_stream_index = i; break;
        }
    }
    if (audio_stream_index < 0) return waveform;
    AVCodecParameters *codecpar = fmt_ctx->streams[audio_stream_index]->codecpar;
    AVCodec *dec = avcodec_find_decoder(codecpar->codec_id);
    AVCodecContext *codec_ctx = avcodec_alloc_context3(dec);
    avcodec_parameters_to_context(codec_ctx, codecpar);
    avcodec_open2(codec_ctx, dec, nullptr);

    SwrContext *swr_ctx = swr_alloc_set_opts(nullptr,
        AV_CH_LAYOUT_MONO, AV_SAMPLE_FMT_FLT, codec_ctx->sample_rate,
        codec_ctx->channel_layout, codec_ctx->sample_fmt, codec_ctx->sample_rate,
        0, nullptr);
    swr_init(swr_ctx);

    AVPacket *pkt = av_packet_alloc();
    AVFrame *frame = av_frame_alloc();
    QVector<float> samples;
    while (av_read_frame(fmt_ctx, pkt) >= 0) {
        if (pkt->stream_index == audio_stream_index) {
            if (avcodec_send_packet(codec_ctx, pkt) == 0) {
                while (avcodec_receive_frame(codec_ctx, frame) == 0) {
                    float *out = new float[frame->nb_samples];
                    swr_convert(swr_ctx, (uint8_t **)&out, frame->nb_samples,
                        (const uint8_t **)frame->data, frame->nb_samples);
                    for (int i = 0; i < frame->nb_samples; ++i)
                        samples << out[i];
                    delete[] out;
                }
            }
        }
        av_packet_unref(pkt);
    }
    av_frame_free(&frame);
    av_packet_free(&pkt);
    swr_free(&swr_ctx);
    avcodec_free_context(&codec_ctx);
    avformat_close_input(&fmt_ctx);

    int step = samples.size() / samplePoints;
    for (int i = 0; i < samplePoints; ++i) {
        float maxAmp = 0;
        for (int j = i * step; j < (i + 1) * step && j < samples.size(); ++j)
            maxAmp = std::max(maxAmp, std::abs(samples[j]));
        waveform.append(maxAmp);
    }
    return waveform;
}
