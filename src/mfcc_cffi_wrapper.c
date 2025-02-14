// src/mfcc_cffi_wrapper.c
#include "mfcc.h"
#include <stdlib.h>
#include "mfcc_cffi_wrapper.h"

// A simple wrapper that constructs an MFCCConfig and calls compute_mfcc.
float* c_compute_mfcc(float* audio_signal,
                      int signal_length,
                      int sample_rate,
                      int n_fft,
                      int n_mels,
                      int n_mfcc,
                      float fmin,
                      float fmax,
                      float preemphasis,
                      int frame_length,
                      int frame_step) {
    MFCCConfig config;
    config.sample_rate = sample_rate;
    config.n_fft = n_fft;
    config.n_mels = n_mels;
    config.n_mfcc = n_mfcc;
    config.fmin = fmin;
    config.fmax = fmax;
    config.preemphasis = preemphasis;
    // Add frame settings (ensure these fields are declared in your MFCCConfig)
    config.frame_length = frame_length;
    config.frame_step = frame_step;

    return compute_mfcc(audio_signal, signal_length, &config);
}

// Free the memory allocated by compute_mfcc.
void free_mfcc(float* mfcc_features) {
    free(mfcc_features);
}