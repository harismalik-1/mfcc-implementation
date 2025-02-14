cmake_minimum_required(VERSION 3.10)
project(mfcc_cffi C)

# Set C standard
set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)

# For RISC-V target, we need to specify Python paths manually
if(CMAKE_SYSTEM_PROCESSOR STREQUAL "riscv64")
    # Use Python from sysroot
    set(SYSROOT "${CMAKE_SOURCE_DIR}/riscv-sysroot")
    set(Python3_INCLUDE_DIRS "${SYSROOT}/usr/include/python3.9")  # Changed to 3.9
    set(Python3_LIBRARIES "${SYSROOT}/usr/lib/libpython3.9.so")   # Changed to 3.9
else()
    # For native build, use find_package
    find_package(Python3 COMPONENTS Development REQUIRED)
endif()

# Add include directories
include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${Python3_INCLUDE_DIRS}
)

# Create a shared library that includes your MFCC code and our cffi wrapper.
add_library(mfcc_cffi SHARED
    src/mfcc.c
    src/mfcc_cffi_wrapper.c
)

# Link with math library if needed.
target_link_libraries(mfcc_cffi PRIVATE m)

# Ensure the output module has the correct name
set_target_properties(mfcc_cffi PROPERTIES
    PREFIX ""
    SUFFIX ".so"
)

# Set static linking for RISC-V target
if(CMAKE_SYSTEM_PROCESSOR STREQUAL "riscv64")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -static")
    set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -static-libgcc")
    # Add sysroot to compiler and linker flags
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} --sysroot=${SYSROOT}")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} --sysroot=${SYSROOT}")
    set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} --sysroot=${SYSROOT}")
endif()