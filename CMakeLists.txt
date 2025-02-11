cmake_minimum_required(VERSION 3.10)
project(asr C)

# Set C standard
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

# Add static linking flag
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -static")

# Add include directories
include_directories(
    ${CMAKE_SOURCE_DIR}/include
)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

# Add source files
add_library(mfcc_lib
    src/mfcc.c
)

# Link libraries
target_link_libraries(mfcc_lib 
    PRIVATE 
    m
)

# Add test executable
add_executable(test_mfcc
    tests/test_mfcc.c
)

target_link_libraries(test_mfcc 
    PRIVATE 
    mfcc_lib
)