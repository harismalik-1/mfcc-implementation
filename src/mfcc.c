#include "mfcc.h"    // for Complex struct and other declarations
#define _USE_MATH_DEFINES
#include <math.h>
#include <stdlib.h>  // for malloc, calloc, free
#include <string.h>  // for memcpy
#include <stdbool.h>  // Add this for bool type

// If M_PI is still not defined, define it manually
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

// Define M_SQRT1_2 if not available (1/√2)
#ifndef M_SQRT1_2
#define M_SQRT1_2 0.70710678118654752440
#endif

// Add top_db clipping constant
#define TOP_DB 80.0f

// Pre-emphasis filter implementation
float* preemphasis(float* signal, int length, float coef) {
    float* emphasized = (float*)malloc(length * sizeof(float));
    
    // First sample remains unchanged
    emphasized[0] = signal[0];
    
    // Apply pre-emphasis filter: y[n] = x[n] - α*x[n-1]
    for (int i = 1; i < length; i++) {
        emphasized[i] = signal[i] - coef * signal[i-1];
    }
    
    return emphasized;
}

// Create Hamming window
float* create_hamming_window(int length) {
    float* window = (float*)malloc(length * sizeof(float));
    for (int i = 0; i < length; i++) {
        // Hamming window formula: 0.54 - 0.46 * cos(2π * n / (N-1))
        window[i] = 0.54f - 0.46f * cosf(2.0f * M_PI * i / (length - 1));
    }
    return window;
}

// Frame the signal into overlapping frames
float** frame_signal(float* signal, int signal_length, int frame_length, int frame_step, int* num_frames) {
    // Calculate number of frames
    *num_frames = 1 + (signal_length - frame_length) / frame_step;
    
    // Allocate memory for frames
    float** frames = (float**)malloc(*num_frames * sizeof(float*));
    for (int i = 0; i < *num_frames; i++) {
        frames[i] = (float*)malloc(frame_length * sizeof(float));
        
        // Copy samples into frame
        int start = i * frame_step;
        for (int j = 0; j < frame_length; j++) {
            if (start + j < signal_length) {
                frames[i][j] = signal[start + j];
            } else {
                frames[i][j] = 0.0f;  // Zero padding if needed
            }
        }
    }
    return frames;
}

// Apply window function to a frame
void apply_window(float* frame, float* window, int frame_length) {
    for (int i = 0; i < frame_length; i++) {
        frame[i] *= window[i];
    }
}

// Add FFT helper functions
void fft_recursive(Complex* x, int n) {
    if (n <= 1) return;

    // Divide
    Complex* even = (Complex*)malloc(n/2 * sizeof(Complex));
    Complex* odd = (Complex*)malloc(n/2 * sizeof(Complex));
    
    for (int i = 0; i < n/2; i++) {
        even[i] = x[2*i];
        odd[i] = x[2*i+1];
    }

    // Conquer
    fft_recursive(even, n/2);
    fft_recursive(odd, n/2);

    // Combine
    for (int k = 0; k < n/2; k++) {
        float angle = -2 * M_PI * k / n;
        Complex t = {
            .real = cosf(angle),
            .imag = sinf(angle)
        };
        Complex temp = {
            .real = t.real * odd[k].real - t.imag * odd[k].imag,
            .imag = t.real * odd[k].imag + t.imag * odd[k].real
        };
        
        x[k].real = even[k].real + temp.real;
        x[k].imag = even[k].imag + temp.imag;
        
        x[k + n/2].real = even[k].real - temp.real;
        x[k + n/2].imag = even[k].imag - temp.imag;
    }

    free(even);
    free(odd);
}

Complex* compute_fft(float* frame, int frame_length) {
    // Ensure frame_length is power of 2
    Complex* fft_input = (Complex*)malloc(frame_length * sizeof(Complex));
    
    // Convert real input to complex
    for (int i = 0; i < frame_length; i++) {
        fft_input[i].real = frame[i];
        fft_input[i].imag = 0.0f;
    }

    // Perform FFT
    fft_recursive(fft_input, frame_length);
    
    return fft_input;
}

float* compute_power_spectrum(Complex* fft_result, int length) {
    float* power = (float*)malloc((length/2 + 1) * sizeof(float));
    
    // Match librosa's scaling: S = |X|^2
    // Remove the division by n_fft that was causing the scale difference
    for (int i = 0; i <= length/2; i++) {
        power[i] = (fft_result[i].real * fft_result[i].real + 
                    fft_result[i].imag * fft_result[i].imag);
        
        // Double all frequencies except DC and Nyquist
        if (i > 0 && i < length/2) {
            power[i] *= 2.0f;
        }
    }
    
    return power;
}

// Update mel scale conversion functions
float hz_to_mel(float hz, MelScaleType scale_type) {
    if (scale_type == MEL_SCALE_HTK) {
        // HTK formula
        return 2595.0f * log10f(1.0f + hz / 700.0f);
    } else {
        // Slaney formula
        float f_min = 0.0f;
        float f_sp = 200.0f / 3.0f;
        float min_log_hz = 1000.0f;
        float min_log_mel = (min_log_hz - f_min) / f_sp;
        float logstep = logf(6.4f) / 27.0f;

        if (hz < min_log_hz) {
            return (hz - f_min) / f_sp;
        } else {
            return min_log_mel + logf(hz / min_log_hz) / logstep;
        }
    }
}

float mel_to_hz(float mel, MelScaleType scale_type) {
    if (scale_type == MEL_SCALE_HTK) {
        // HTK formula
        return 700.0f * (powf(10.0f, mel / 2595.0f) - 1.0f);
    } else {
        // Slaney formula
        float f_min = 0.0f;
        float f_sp = 200.0f / 3.0f;
        float min_log_hz = 1000.0f;
        float min_log_mel = (min_log_hz - f_min) / f_sp;
        float logstep = logf(6.4f) / 27.0f;

        if (mel < min_log_mel) {
            return f_min + f_sp * mel;
        } else {
            return min_log_hz * expf(logstep * (mel - min_log_mel));
        }
    }
}

// Update filterbank creation with normalization
float** create_mel_filterbank(int fft_length, int sample_rate, int n_mels, 
                            float fmin, float fmax, MelScaleType scale_type, 
                            bool normalize) {
    float** filterbank = (float**)malloc(n_mels * sizeof(float*));
    
    // Convert frequency range to mel scale
    float mel_min = hz_to_mel(fmin, scale_type);
    float mel_max = hz_to_mel(fmax, scale_type);
    float mel_step = (mel_max - mel_min) / (n_mels + 1);
    
    // Create center frequencies in mel scale
    float* mel_points = (float*)malloc((n_mels + 2) * sizeof(float));
    for (int i = 0; i < n_mels + 2; i++) {
        mel_points[i] = mel_min + i * mel_step;
    }
    
    // Convert back to Hz
    float* hz_points = (float*)malloc((n_mels + 2) * sizeof(float));
    for (int i = 0; i < n_mels + 2; i++) {
        hz_points[i] = mel_to_hz(mel_points[i], scale_type);
    }
    
    // Convert to FFT bin numbers
    int* bin_points = (int*)malloc((n_mels + 2) * sizeof(int));
    for (int i = 0; i < n_mels + 2; i++) {
        bin_points[i] = (int)floorf((fft_length + 1) * hz_points[i] / sample_rate);
    }
    
    // Create triangular filters
    for (int i = 0; i < n_mels; i++) {
        filterbank[i] = (float*)calloc(1 + fft_length/2, sizeof(float));
        
        for (int j = bin_points[i]; j < bin_points[i+2]; j++) {
            if (j < bin_points[i+1]) {
                filterbank[i][j] = (j - bin_points[i]) / 
                                 (float)(bin_points[i+1] - bin_points[i]);
            } else {
                filterbank[i][j] = (bin_points[i+2] - j) /
                                 (float)(bin_points[i+2] - bin_points[i+1]);
            }
        }

        // Apply Slaney normalization if requested
        if (normalize) {
            float enorm = 2.0f / (hz_points[i+2] - hz_points[i]);
            for (int j = 0; j <= fft_length/2; j++) {
                filterbank[i][j] *= enorm;
            }
        }
    }
    
    // Clean up
    free(mel_points);
    free(hz_points);
    free(bin_points);
    
    return filterbank;
}

void apply_filterbank(float* power_spectrum, float** filterbank, int n_filters, int n_fft, float* mel_energies) {
    for (int i = 0; i < n_filters; i++) {
        float energy = 0.0f;
        
        for (int j = 0; j <= n_fft/2; j++) {
            energy += power_spectrum[j] * filterbank[i][j];
        }
        
        // Remove normalization by filter response
        mel_energies[i] = energy;
    }
}

// Add DCT implementation
void apply_dct(float* mel_energies, float* mfcc_features, int n_mels, int n_mfcc) {
    for (int i = 0; i < n_mfcc; i++) {
        float sum = 0.0f;
        // Use librosa's DCT normalization
        float dct_norm = (i == 0) ? sqrtf(1.0f / n_mels) : sqrtf(2.0f / n_mels);
        
        for (int j = 0; j < n_mels; j++) {
            float angle = M_PI * i * (j + 0.5f) / n_mels;
            sum += mel_energies[j] * cosf(angle);
        }
        mfcc_features[i] = dct_norm * sum;
    }
}

// Implementation of functions declared in mfcc.h
float* compute_mfcc(float* audio_signal, int signal_length, MFCCConfig* config) {
    // Apply pre-emphasis
    float* emphasized_signal = preemphasis(audio_signal, signal_length, config->preemphasis);
    
    // Create Hamming window
    float* window = create_hamming_window(config->frame_length);
    
    // Frame the signal
    int num_frames;
    float** frames = frame_signal(emphasized_signal, signal_length, 
                                config->frame_length, config->frame_step, &num_frames);
    
    // Apply windowing to each frame
    for (int i = 0; i < num_frames; i++) {
        apply_window(frames[i], window, config->frame_length);
    }
    
    // After windowing, compute FFT for each frame
    Complex** ffts = (Complex**)malloc(num_frames * sizeof(Complex*));
    float** power_specs = (float**)malloc(num_frames * sizeof(float*));
    
    for (int i = 0; i < num_frames; i++) {
        ffts[i] = compute_fft(frames[i], config->frame_length);
        power_specs[i] = compute_power_spectrum(ffts[i], config->frame_length);
    }
    
    // Create mel filterbank with new parameters
    float** filterbank = create_mel_filterbank(config->frame_length, 
                                             config->sample_rate,
                                             config->n_mels,
                                             config->fmin,
                                             config->fmax,
                                             config->mel_scale_type,
                                             config->normalize_mel);
    
    // Calculate total number of coefficients (n_mfcc * num_frames)
    int total_coefficients = config->n_mfcc * num_frames;
    
    // Allocate memory for all frames' MFCCs
    float* result = (float*)malloc(total_coefficients * sizeof(float));
    float* mel_energies = (float*)malloc(config->n_mels * sizeof(float));
    
    // Process each frame
    for (int frame = 0; frame < num_frames; frame++) {
        // Apply filterbank and compute log energy for current frame
        apply_filterbank(power_specs[frame], filterbank, config->n_mels, 
                        config->frame_length, mel_energies);
        
        // Take log of mel energies
        float min_power = 1e-10f;  // librosa's default amin
        for (int i = 0; i < config->n_mels; i++) {
            // Match librosa's power_to_db conversion
            float val = fmaxf(mel_energies[i], min_power);
            mel_energies[i] = 10.0f * log10f(val);
            
            // Apply top_db clipping
            if (mel_energies[i] < -TOP_DB) {
                mel_energies[i] = -TOP_DB;
            }
        }
        
        // Apply DCT to get MFCC features for current frame
        apply_dct(mel_energies, &result[frame * config->n_mfcc], 
                 config->n_mels, config->n_mfcc);
    }
    
    // Clean up
    free(mel_energies);
    for (int i = 0; i < config->n_mels; i++) {
        free(filterbank[i]);
    }
    free(filterbank);
    
    // Clean up
    for (int i = 0; i < num_frames; i++) {
        free(ffts[i]);
        free(power_specs[i]);
        free(frames[i]);
    }
    free(ffts);
    free(power_specs);
    free(frames);
    free(window);
    free(emphasized_signal);
    
    return result;
}