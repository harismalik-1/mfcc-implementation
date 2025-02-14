import os
import cffi

# Create a new FFI interface
ffi = cffi.FFI()

# Declare the interface of the C functions available in the shared library.
ffi.cdef("""
    float* c_compute_mfcc(float* audio_signal, int signal_length,
                          int sample_rate, int n_fft, int n_mels, int n_mfcc,
                          float fmin, float fmax, float preemphasis,
                          int frame_length, int frame_step);
    void free_mfcc(float* mfcc_features);
""")

# Determine the path to the shared library. We assume mfcc_cffi.so is in the same directory as this file.
_lib_path = os.path.join(os.path.dirname(__file__), "mfcc_cffi.so")
if not os.path.exists(_lib_path):
    raise FileNotFoundError("Shared library mfcc_cffi.so not found in the module directory.")

# Load the shared library.
lib = ffi.dlopen(_lib_path)

def compute_mfcc(signal, sample_rate=16000, n_fft=512, n_mels=80,
                 n_mfcc=40, fmin=0.0, fmax=8000.0, preemphasis=0.97,
                 frame_length=400, frame_step=160):
    """
    Compute MFCC features for a given audio signal.
    
    Parameters:
      signal         : List of floats (audio samples)
      sample_rate    : Sampling rate in Hz (default: 16000)
      n_fft          : Number of FFT bins (default: 512)
      n_mels         : Number of mel filters (default: 80)
      n_mfcc         : Number of MFCC coefficients (default: 40)
      fmin           : Minimum frequency for mel filter bank (default: 0.0)
      fmax           : Maximum frequency for mel filter bank (default: 8000.0)
      preemphasis    : Preemphasis factor (default: 0.97)
      frame_length   : Frame length for analysis (default: 400)
      frame_step     : Step size or hop length (default: 160)
      
    Returns:
      A list of `n_mfcc` computed MFCC features for the input signal (one frame).
    """
    # Allocate a C array from the Python list
    c_signal = ffi.new("float[]", signal)
    
    # Call the C function to calculate MFCC features
    result_ptr = lib.c_compute_mfcc(c_signal, len(signal),
                                    sample_rate, n_fft, n_mels, n_mfcc,
                                    fmin, fmax, preemphasis,
                                    frame_length, frame_step)
    
    # Retrieve the MFCC values from the returned pointer.
    # We assume that the result is an array of n_mfcc floats.
    result = [ffi.cast("float*", result_ptr)[i] for i in range(n_mfcc)]
    
    # Free the allocated memory on the C side.
    lib.free_mfcc(result_ptr)
    
    return result

# If you run this module directly, perform a simple test.
if __name__ == "__main__":
    import math
    sample_rate = 16000
    # Create a dummy signal: a 1-second 440 Hz sine wave.
    signal = [math.sin(2 * math.pi * 440 * i / sample_rate) for i in range(sample_rate)]
    
    mfcc_features = compute_mfcc(signal)
    print("Computed MFCC Features:", mfcc_features) 