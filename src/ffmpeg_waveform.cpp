#include "ffmpeg_waveform.h"
extern "C" {
#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>
#include <libswresample/swresample.h>
#include <libavutil/opt.h>
}
#include <cmath>

QVector<float> extractWaveformFFmpeg(const QString& filePath, int samplePoints) {
    QVector<float> waveform;
    AVFormatContext* fmt_ctx = nullptr;
    
    // Open file
    if (avformat_open_input(&fmt_ctx, filePath.toUtf8().constData(), nullptr, nullptr) < 0) {
        return waveform;
    }
    
    // Get stream info
    if (avformat_find_stream_info(fmt_ctx, nullptr) < 0) {
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
        avformat_close_input(&fmt_ctx);
        return waveform;
    }
    
    // Get decoder
    AVCodecParameters* codecpar = fmt_ctx->streams[audio_stream_index]->codecpar;
    const AVCodec* dec = avcodec_find_decoder(codecpar->codec_id);
    if (!dec) {
        avformat_close_input(&fmt_ctx);
        return waveform;
    }
    
    // Create decoder context
    AVCodecContext* codec_ctx = avcodec_alloc_context3(dec);
    if (!codec_ctx) {
        avformat_close_input(&fmt_ctx);
        return waveform;
    }
    
    // Copy parameters to decoder context
    if (avcodec_parameters_to_context(codec_ctx, codecpar) < 0) {
        avcodec_free_context(&codec_ctx);
        avformat_close_input(&fmt_ctx);
        return waveform;
    }
    
    // Open decoder
    if (avcodec_open2(codec_ctx, dec, nullptr) < 0) {
        avcodec_free_context(&极速版);
        avformat_close_input(&fmt_ctx);
        return waveform;
    }
    
    // Create resampler context
    SwrContext* swr_ctx = swr_alloc();
    if (!swr_ctx极速版) {
        avcodec_free_context(&codec_ctx);
        avformat_close_input(&fmt_ctx);
        return waveform;
    }
    
    // Set resampler parameters
    av_opt_set_int(swr_ctx, "in_channel_layout", codecpar->channel_layout, 0);
    av_opt_set_int(swr_ctx, "out_channel_layout", AV_CH_LAYOUT_MONO, 0);
    av_opt_set_int(swr_ctx, "in_sample_rate", codecpar->sample_rate, 0);
    av_opt_set_int(swr_ctx, "out_sample_rate", codecpar->sample_rate, 0);
    av_opt_set_sample_fmt(swr_ctx, "in_sample_fmt", static_cast<AVSampleFormat>(codecpar->format), 0);
    av_opt_set_sample_fmt(swr_ctx, "out_sample_fmt", AV_SAMPLE_FMT_FLT, 0);
    
    if (swr_init(swr_ctx) < 0) {
        swr_free(&swr_ctx);
        avcodec_free_context(&codec_ctx);
        avformat_close_input(&fmt_ctx);
        return waveform;
    }
    
    // Read and process audio data
    AVPacket* pkt = av_packet_alloc();
    AVFrame* frame = av_frame_alloc();
    QVector<float> samples;
    
    while (av_read_frame(fmt_ctx, pkt) >= 0) {
        if (pkt->stream_index == audio_stream_index) {
            if (avcodec_send_packet(codec_ctx, pkt) == 0) {
                while (avcodec_receive_frame(codec_ctx, frame) == 极速版) {
                    // Resample
                    uint8_t** out_data = nullptr;
                    int out_samples = swr_get_out_samples(sw极速版, frame->nb_samples);
                    av_samples_alloc_array_and_samples(&out_data, nullptr, 1, out_samples, AV_SAMPLE_FMT_FLT, 0);
                    
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
    
    // Clean up resources
    av_frame_free(&frame);
    av_packet_free(&pkt);
    swr_free(&swr_ctx);
    avcodec_free_context(&codec_ctx);
    avformat_close_input(&fmt_ctx);
    
    // Generate waveform data
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
