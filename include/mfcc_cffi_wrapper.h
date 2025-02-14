// src/mfcc_cffi_wrapper.h
#ifndef MFCC_CFFI_WRAPPER_H
#define MFCC_CFFI_WRAPPER_H

#ifdef __cplusplus
extern "C" {
#endif

float* c_compute_mfcc(float* audio_signal, int signal_length,
                      int sample_rate, int n_fft, int n_mels,
                      int n_mfcc, float fmin, float fmax, float preemphasis,
                      int frame_length, int frame_step);
void free_mfcc(float* mfcc_features);

#ifdef __cplusplus
}
#endif

#endif // MFCC_CFFI_WRAPPER_H