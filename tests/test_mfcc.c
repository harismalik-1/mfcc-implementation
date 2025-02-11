#include <stdio.h>
#include <math.h>
#include "mfcc.h"

int main() {
    printf("Testing MFCC implementation...\n");
    
    // Test basic audio parameters
    MFCCConfig config = {
        .sample_rate = 16000,
        .n_fft = 512,
        .n_mels = 80,
        .n_mfcc = 40,
        .fmin = 0.0f,
        .fmax = 8000.0f,
        .preemphasis = 0.97f,
        .frame_length = 400,  // 25ms * 16000Hz
        .frame_step = 160     // 10ms * 16000Hz
    };
    
    // Create a simple test signal (sine wave)
    int signal_length = 16000;
    float test_signal[16000];
    for (int i = 0; i < signal_length; i++) {
        test_signal[i] = sin(2 * M_PI * 440 * i / 16000.0);
    }
    
    // Test pre-emphasis directly
    printf("\nTesting pre-emphasis filter...\n");
    float* emphasized = preemphasis(test_signal, signal_length, config.preemphasis);
    printf("First 5 samples before pre-emphasis:\n");
    for (int i = 0; i < 5; i++) {
        printf("Sample[%d] = %f\n", i, test_signal[i]);
    }
    printf("\nFirst 5 samples after pre-emphasis:\n");
    for (int i = 0; i < 5; i++) {
        printf("Sample[%d] = %f\n", i, emphasized[i]);
    }
    free(emphasized);
    
    // Test full MFCC computation
    printf("\nTesting MFCC computation...\n");
    float* mfcc_features = compute_mfcc(test_signal, signal_length, &config);
    printf("First 5 MFCC coefficients:\n");
    for (int i = 0; i < 5; i++) {
        printf("MFCC[%d] = %f\n", i, mfcc_features[i]);
    }
    
    free(mfcc_features);
    return 0;
}