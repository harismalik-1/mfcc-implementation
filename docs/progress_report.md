# MFCC Implementation for MilkV RISC-V Board - Progress Report

## Table of Contents
1. [Project Overview](#project-overview)
2. [Project Structure](#project-structure)
3. [Development Timeline](#development-timeline)
4. [Implementation Details](#implementation-details)
5. [Challenges and Solutions](#challenges-and-solutions)
6. [Next Steps](#next-steps)

## Project Overview
Implementation of Mel-frequency cepstral coefficients (MFCC) computation on MilkV Duo RISC-V board for speech recognition tasks.

### Goals
- Implement MFCC computation without heavy dependencies
- Optimize for RISC-V architecture
- Create reusable audio feature extraction pipeline
- Alternative to installing torchaudio on resource-constrained RISC-V boards

## Project Structure
asr/
├── CMakeLists.txt          # Build system configuration
├── cmake/
│   └── riscv64.cmake      # RISC-V toolchain configuration
├── docs/
│ └── progress_report.md # This documentation
├── include/
│ └── mfcc.h # MFCC interface declarations
├── src/
│ └── mfcc.c # MFCC implementation
└── tests/
└── test_mfcc.c # Test program


## Development Timeline

### Phase 1: Initial Setup (Completed)
1. Project structure creation
2. Cross-compilation environment setup
3. Basic test implementation

#### Key Code Components

##### Configuration Structure (mfcc.h)
c
typedef struct {
int sample_rate; // Audio sample rate (16000 Hz)
int n_fft; // FFT size (512)
int n_mels; // Number of mel filterbanks (80)
int n_mfcc; // Number of MFCC coefficients (40)
float fmin; // Minimum frequency (0 Hz)
float fmax; // Maximum frequency (8000 Hz)
} MFCCConfig;

##### Initial Test Implementation (test_mfcc.c)
c
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
.fmax = 8000.0f
};
// Create a simple test signal (sine wave)
int signal_length = 16000; // 1 second of audio
float test_signal[16000];
for (int i = 0; i < signal_length; i++) {
test_signal[i] = sin(2 M_PI 440 i / 16000.0); // 440 Hz tone
}
float mfcc_features = compute_mfcc(test_signal, signal_length, &config);
printf("First 5 MFCC coefficients:\n");
for (int i = 0; i < 5; i++) {
printf("MFCC[%d] = %f\n", i, mfcc_features[i]);
}
free(mfcc_features);
return 0;
}

##### Initial MFCC Implementation (mfcc.c)
c
#include "mfcc.h"
#include <math.h>
float compute_mfcc(float audio_signal, int signal_length, MFCCConfig config) {
// Temporary stub implementation without FFTW
float result = (float)malloc(config->n_mfcc sizeof(float));
// Just for testing - compute simple energy features
for (int i = 0; i < config->n_mfcc; i++) {
float sum = 0;
for (int j = 0; j < signal_length; j++) {
sum += audio_signal[j] audio_signal[j];
}
result[i] = sqrt(sum / signal_length);
}
return result;
}
float create_mel_filterbank(MFCCConfig config) {
// Temporary stub
return NULL;
}

##### Build System (CMakeLists.txt)
cmake
cmake_minimum_required(VERSION 3.10)
project(asr C)
Set C standard
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
Add include directories
include_directories(
${CMAKE_SOURCE_DIR}/include
)
Add source files
add_library(mfcc_lib
src/mfcc.c
)
Link libraries
target_link_libraries(mfcc_lib
PRIVATE
m
)
Add test executable
add_executable(test_mfcc
tests/test_mfcc.c
)
target_link_libraries(test_mfcc
PRIVATE
mfcc_lib
)

##### RISC-V Toolchain Configuration (cmake/riscv64.cmake)
cmake
set(CMAKE_SYSTEM_NAME Linux)
set(CMAKE_SYSTEM_PROCESSOR riscv64)
set(CMAKE_C_COMPILER riscv64-linux-gnu-gcc)
set(CMAKE_CXX_COMPILER riscv64-linux-gnu-g++)
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE ONLY)


## Challenges and Solutions

### 1. Cross-Compilation Setup
**Challenge**: CMake couldn't find RISC-V compiler
**Solution**: 
bash
Install RISC-V toolchain
sudo apt-get install gcc-riscv64-linux-gnu
Update toolchain file (cmake/riscv64.cmake)
set(CMAKE_SYSTEM_NAME Linux)
set(CMAKE_SYSTEM_PROCESSOR riscv64)
set(CMAKE_C_COMPILER riscv64-linux-gnu-gcc)
set(CMAKE_CXX_COMPILER riscv64-linux-gnu-g++)

### 2. Library Dependencies
**Challenge**: FFTW library dependency caused linking errors
**Solution**: Temporarily removed external dependencies for initial testing
cmake
Simplified CMakeLists.txt
target_link_libraries(mfcc_lib
PRIVATE
m
)

### 3. Dynamic Linking
**Challenge**: Missing dynamic linker on MilkV board
**Error**: `-sh: ./test_mfcc: not found`
**Solution**: Successfully ran with static linking and proper permissions
bash
chmod +x test_mfcc
./test_mfcc

## Implementation Details

### Build Process
bash
On development machine
mkdir build_riscv
cd build_riscv
cmake -DCMAKE_TOOLCHAIN_FILE=../cmake/riscv64.cmake ..
make
Transfer to MilkV

### Test Results
Current output shows successful cross-compilation and basic functionality:
Testing MFCC implementation...
First 5 MFCC coefficients:
MFCC[0] = 0.707107
MFCC[1] = 0.707107
MFCC[2] = 0.707107
MFCC[3] = 0.707107
MFCC[4] = 0.707107


### Phase 2: Pre-emphasis Implementation (Completed)

#### Implementation Details
1. **Pre-emphasis Filter**
c
float preemphasis(float signal, int length, float coef) {
float emphasized = (float)malloc(length sizeof(float));
// First sample remains unchanged
emphasized[0] = signal[0];
// Apply pre-emphasis filter: y[n] = x[n] - αx[n-1]
for (int i = 1; i < length; i++) {
emphasized[i] = signal[i] - coef signal[i-1];
}
return emphasized;
}

#### Configuration Updates
Added pre-emphasis coefficient to MFCCConfig:
c
typedef struct {
// ... existing fields ...
float preemphasis; // Pre-emphasis coefficient (typically 0.97)
} MFCCConfig;

#### Test Results
Testing MFCC implementation...
Testing pre-emphasis filter...
First 5 samples before pre-emphasis:
Sample[0] = 0.000000
Sample[1] = 0.171929
Sample[2] = 0.338738
Sample[3] = 0.495459
Sample[4] = 0.637424
First 5 samples after pre-emphasis:
Sample[0] = 0.000000
Sample[1] = 0.171929
Sample[2] = 0.171967
Sample[3] = 0.166883
Sample[4] = 0.156829
Testing MFCC computation...
First 5 MFCC coefficients:
MFCC[0] = 0.122033
MFCC[1] = 0.122033
MFCC[2] = 0.122033
MFCC[3] = 0.122033
MFCC[4] = 0.122033


#### Analysis
1. **Pre-emphasis Effect**
   - First sample remains unchanged (no previous sample)
   - Subsequent samples show high-frequency emphasis
   - Formula: y[n] = x[n] - 0.97*x[n-1]
   - Coefficient 0.97 is standard for speech processing

2. **Signal Changes**
   - Original signal: Pure 440 Hz sine wave
   - After pre-emphasis: Enhanced high-frequency components
   - Energy values reduced due to subtraction operation

3. **Memory Management**
   - Proper allocation for emphasized signal
   - Clean deallocation after use
   - No memory leaks in testing

#### Build System Updates
1. **Static Linking**
cmake
Added to CMakeLists.txt
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -static")

2. **Cross-compilation**
   - Successfully building for RISC-V
   - Static linking resolves dynamic library dependencies
   - Executable runs correctly on MilkV board

#### Current Status
- ✅ Basic project structure
- ✅ Cross-compilation pipeline
- ✅ Pre-emphasis implementation
- ✅ Memory management
- ✅ Initial testing framework