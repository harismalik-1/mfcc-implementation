#ifndef MFCC_H
#define MFCC_H

#include <stdlib.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

// Complex number structure - MUST be defined before any function declarations
typedef struct Complex {
    float real;
    float imag;
} Complex;

// Mel scale type enumeration
typedef enum {
    MEL_SCALE_HTK,
    MEL_SCALE_SLANEY
} MelScaleType;

// MFCC configuration structure
typedef struct {
    float preemphasis;      // Pre-emphasis coefficient
    int frame_length;       // Length of each frame
    int frame_step;         // Number of samples between frames
    int sample_rate;        // Audio sample rate
    int n_mels;            // Number of mel bands
    int n_mfcc;            // Number of MFCC coefficients
    float fmin;            // Minimum frequency
    float fmax;            // Maximum frequency
    int n_fft;             // FFT size
    MelScaleType mel_scale_type;  // Type of mel scale to use
    bool normalize_mel;     // Whether to normalize mel filterbanks
} MFCCConfig;

// Function declarations
float* preemphasis(float* signal, int length, float coef);
float* compute_mfcc(float* audio_signal, int signal_length, MFCCConfig* config);
float** create_mel_filterbank(int fft_length, int sample_rate, int n_mels, 
                            float fmin, float fmax, MelScaleType scale_type, 
                            bool normalize);
float* create_hamming_window(int length);
float** frame_signal(float* signal, int signal_length, int frame_length, int frame_step, int* num_frames);
void apply_window(float* frame, float* window, int frame_length);

// Helper function declarations
Complex* compute_fft(float* frame, int frame_length);
void fft_recursive(Complex* x, int n);
float* compute_power_spectrum(Complex* fft_result, int length);
float hz_to_mel(float hz, MelScaleType scale_type);
float mel_to_hz(float mel, MelScaleType scale_type);
void apply_filterbank(float* power_spectrum, float** filterbank, int n_filters, int n_fft, float* mel_energies);

// Add new DCT function declaration
void apply_dct(float* mel_energies, float* mfcc_features, int n_mels, int n_mfcc);

#ifdef __cplusplus
}
#endif

#endif // MFCC_H