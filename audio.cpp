#include "audio.h"
#include "config.h"

snd_pcm_t *pcm_handle;
u32 sample_rate;
u32 channels;
u32 depth;
u32 max_samples;
u32 samples_decoded = 0;

std::vector<s32> decoder_output;
// Output buffers
std::vector<s16> alsa_buffer_16;
std::vector<s32> alsa_buffer_32;
flac::FLACDecoderResult res;
// Increased input buffer size
std::vector<u8> read_buffer(DISK_BUFFER_SIZE);
flac::FLACDecoder decoder(read_buffer.data(), DISK_BUFFER_SIZE, MIN_BUFFER_SIZE);

snd_pcm_t* setup_alsa(u32 sample_rate, u32 channels, u32 bits_per_sample) {
    snd_pcm_t *pcm_handle;
    snd_pcm_hw_params_t *params;
    int err;

    if ((err = snd_pcm_open(&pcm_handle, "default", SND_PCM_STREAM_PLAYBACK, 0)) < 0) {
        std::cerr << "Cannot open audio device default (" << snd_strerror(err) << ")" << std::endl;
        return nullptr;
    }

    if ((err = snd_pcm_hw_params_malloc(&params)) < 0) {
        return nullptr;
    }

    if ((err = snd_pcm_hw_params_any(pcm_handle, params)) < 0) {
        return nullptr;
    }

    if ((err = snd_pcm_hw_params_set_access(pcm_handle, params, SND_PCM_ACCESS_RW_INTERLEAVED)) < 0) {
        return nullptr;
    }

    // --- Dynamic Format Selection ---
    snd_pcm_format_t format;
    if (bits_per_sample == 8) {
        format = SND_PCM_FORMAT_U8;
    } else if (bits_per_sample == 16) {
        format = SND_PCM_FORMAT_S16_LE;
    } else {
        // 24-bit and 32-bit use S32_LE container
        format = SND_PCM_FORMAT_S32_LE;
    }

    if ((err = snd_pcm_hw_params_set_format(pcm_handle, params, format)) < 0) {
        std::cerr << "Device doesn't support requested format: " << snd_strerror(err) << std::endl;
        return nullptr;
    }

    unsigned int rate = sample_rate;
    if ((err = snd_pcm_hw_params_set_rate_near(pcm_handle, params, &rate, 0)) < 0) {
        return nullptr;
    }

    if ((err = snd_pcm_hw_params_set_channels(pcm_handle, params, channels)) < 0) {
        return nullptr;
    }

    unsigned int buffer_time = 50000; 
    snd_pcm_hw_params_set_buffer_time_near(pcm_handle, params, &buffer_time, 0);

    if ((err = snd_pcm_hw_params(pcm_handle, params)) < 0) {
        std::cerr << "Cannot set parameters (" << snd_strerror(err) << ")" << std::endl;
        return nullptr;
    }

    snd_pcm_hw_params_free(params);

    if ((err = snd_pcm_prepare(pcm_handle)) < 0) {
        return nullptr;
    }

    return pcm_handle;
}

void init_audio() {
    if (decoder.read_header() != flac::FLAC_DECODER_SUCCESS) {
        std::cerr << "Failed to read FLAC header." << std::endl;
        return;
    }

    sample_rate = decoder.get_sample_rate();
    channels = decoder.get_num_channels();
    depth = decoder.get_sample_depth();

    std::cout << "Playing: " << sample_rate << "Hz | " 
              << channels << " Channels | " 
              << depth << " Bit" << std::endl;

    pcm_handle = setup_alsa(sample_rate, channels, depth);
    if (!pcm_handle) return;

    max_samples = decoder.get_output_buffer_size();
    decoder_output.resize(max_samples);

    if (depth <= 16) {
        alsa_buffer_16.resize(max_samples);
    } else {
        alsa_buffer_32.resize(max_samples);
    }
}

void update_audio() {
    res = decoder.decode_frame(decoder_output.data(), &samples_decoded);

    if (res == flac::FLAC_DECODER_NO_MORE_FRAMES) {
        done = 1;
        return;
    }
    if (res != flac::FLAC_DECODER_SUCCESS) {
        std::cerr << "Error decoding frame: " << res << std::endl;
        done = 1;
        return;
    }

    if (samples_decoded > 0) {
        snd_pcm_uframes_t frames = samples_decoded / channels;
        void* pcm_buf_ptr = nullptr;

        if (depth <= 16) {
            for (size_t i = 0; i < samples_decoded; i++) {
                alsa_buffer_16[i] = (s16)decoder_output[i];
            }
            pcm_buf_ptr = alsa_buffer_16.data();
        } else if (depth == 24) {
            // 24-bit in 32-bit container.
            // Shift 8 bits left using unsigned cast to avoid undefined behavior on signed negative shifts
            for (size_t i = 0; i < samples_decoded; i++) {
                alsa_buffer_32[i] = (s32)((u32)decoder_output[i] << 8);
            }
            pcm_buf_ptr = alsa_buffer_32.data();
        } else {
            // 32-bit (rare)
            pcm_buf_ptr = decoder_output.data();
        }

        snd_pcm_sframes_t frames_written = snd_pcm_writei(pcm_handle, pcm_buf_ptr, frames);

        if (frames_written == -EPIPE) {
            // Underrun occurred
            // std::cerr << "XRUN" << std::endl; 
            snd_pcm_prepare(pcm_handle);
        } else if (frames_written < 0) {
            std::cerr << "ALSA write failed: " << snd_strerror(frames_written) << std::endl;
            done = 1;
            return;
        }
    }
}

void close_audio() {
    if (pcm_handle) {
        snd_pcm_drain(pcm_handle);
        snd_pcm_close(pcm_handle);
        pcm_handle = nullptr;
    }
}