import numpy as np
import soundfile as sf

# Generate a test signal (1 second of 440Hz sine wave)
sample_rate = 16000
t = np.linspace(0, 1, sample_rate)
signal = np.sin(2 * np.pi * 440 * t)

# Save as WAV file
sf.write('test.wav', signal, sample_rate) 