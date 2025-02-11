import numpy as np
import librosa
import mfcc  # our module
import matplotlib.pyplot as plt
import soundfile as sf

def generate_test_signals():
    """Generate different test signals"""
    sr = 16000
    duration = 1.0
    t = np.linspace(0, duration, int(sr * duration))
    
    signals = {
        'sine_440': np.sin(2 * np.pi * 440 * t),  # A4 note
        'sine_mix': np.sin(2 * np.pi * 440 * t) + 0.5 * np.sin(2 * np.pi * 880 * t),  # A4 + A5
        'chirp': librosa.chirp(sr=sr, duration=duration, fmin=20, fmax=8000),  # Frequency sweep
        'white_noise': np.random.randn(len(t)),
        'silence': np.zeros_like(t),
        'impulse': np.zeros_like(t),  # Single impulse
    }
    signals['impulse'][len(t)//2] = 1.0
    
    return signals, sr

def compare_mfccs(signal, sr, name=""):
    """Compare MFCC computation between librosa and our implementation"""
    # Parameters
    n_mfcc = 40
    n_mels = 80
    n_fft = 512
    
    # Compute librosa MFCCs
    mfcc_librosa = librosa.feature.mfcc(y=signal, sr=sr,
                                       n_mfcc=n_mfcc,
                                       n_fft=n_fft,
                                       n_mels=n_mels)
    
    # Compute our MFCCs
    mfcc_ours = np.array(mfcc.compute_mfcc(signal.tolist(), sr, n_fft, n_mels, 
                                          n_mfcc, 0, 8000, 0.97))
    
    # Plot comparison
    plt.figure(figsize=(12, 8))
    plt.suptitle(f'MFCC Comparison - {name}')
    
    plt.subplot(211)
    plt.title('Librosa MFCC (First Frame)')
    plt.imshow(mfcc_librosa[:, 0:1], aspect='auto', origin='lower')
    plt.colorbar()
    
    plt.subplot(212)
    plt.title('Our MFCC')
    plt.imshow(mfcc_ours.reshape(-1, 1), aspect='auto', origin='lower')
    plt.colorbar()
    
    plt.tight_layout()
    plt.savefig(f'mfcc_comparison_{name}.png')
    plt.close()
    
    # Print statistics
    print(f"\nTest Signal: {name}")
    print("Librosa MFCC stats (First Frame):")
    print(f"Mean: {np.mean(mfcc_librosa[:, 0]):.4f}")
    print(f"Std: {np.std(mfcc_librosa[:, 0]):.4f}")
    print(f"Min: {np.min(mfcc_librosa[:, 0]):.4f}")
    print(f"Max: {np.max(mfcc_librosa[:, 0]):.4f}")
    
    print("\nOur MFCC stats:")
    print(f"Mean: {np.mean(mfcc_ours):.4f}")
    print(f"Std: {np.std(mfcc_ours):.4f}")
    print(f"Min: {np.min(mfcc_ours):.4f}")
    print(f"Max: {np.max(mfcc_ours):.4f}")
    
    # Compute correlation with first frame
    correlation = np.corrcoef(mfcc_librosa[:, 0], mfcc_ours)[0,1]
    print(f"\nCorrelation: {correlation:.4f}")
    
    # Print coefficient-wise comparison
    print("\nFirst 5 coefficients comparison:")
    print("Coeff\tLibrosa\t\tOurs\t\tDiff")
    print("-" * 50)
    for i in range(5):
        librosa_val = mfcc_librosa[i, 0]
        our_val = mfcc_ours[i]
        diff = librosa_val - our_val
        print(f"{i}\t{librosa_val:8.4f}\t{our_val:8.4f}\t{diff:8.4f}")
    
    return correlation

def run_all_tests():
    signals, sr = generate_test_signals()
    correlations = {}
    
    for name, signal in signals.items():
        print(f"\n{'='*50}")
        print(f"Testing {name}")
        print('='*50)
        correlations[name] = compare_mfccs(signal, sr, name)
    
    print("\nSummary of correlations:")
    for name, corr in correlations.items():
        print(f"{name}: {corr:.4f}")

if __name__ == "__main__":
    run_all_tests() 