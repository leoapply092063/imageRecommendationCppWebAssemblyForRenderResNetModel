cmake_minimum_required(VERSION 3.16)
project(ImageFeatureExtractor)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find ONNX Runtime - try multiple methods
find_package(PkgConfig QUIET)
if(PkgConfig_FOUND)
    pkg_check_modules(ONNXRUNTIME QUIET onnxruntime)
endif()

# If pkg-config didn't find it, try manual paths
if(NOT ONNXRUNTIME_FOUND)
    # Check for ONNX Runtime in common locations
    set(ONNXRUNTIME_POSSIBLE_PATHS
        "/opt/onnxruntime"
        "/usr/local/onnxruntime"
        "/usr/onnxruntime"
        "${CMAKE_CURRENT_SOURCE_DIR}/onnxruntime"
    )
    
    foreach(PATH ${ONNXRUNTIME_POSSIBLE_PATHS})
        if(EXISTS "${PATH}/include/onnxruntime_cxx_api.h")
            set(ONNXRUNTIME_INCLUDE_DIRS "${PATH}/include")
            set(ONNXRUNTIME_LIBRARIES "${PATH}/lib/libonnxruntime.so")
            set(ONNXRUNTIME_FOUND TRUE)
            break()
        endif()
    endforeach()
endif()

# If still not found, try system installation
if(NOT ONNXRUNTIME_FOUND)
    find_path(ONNXRUNTIME_INCLUDE_DIRS onnxruntime_cxx_api.h
        PATHS /usr/include /usr/local/include
    )
    find_library(ONNXRUNTIME_LIBRARIES onnxruntime
        PATHS /usr/lib /usr/local/lib
    )
    if(ONNXRUNTIME_INCLUDE_DIRS AND ONNXRUNTIME_LIBRARIES)
        set(ONNXRUNTIME_FOUND TRUE)
    endif()
endif()

if(NOT ONNXRUNTIME_FOUND)
    message(FATAL_ERROR "ONNX Runtime not found. Please install it or ensure it's in the correct location.")
endif()

# Include directories
include_directories(${ONNXRUNTIME_INCLUDE_DIRS})

# Add executable
add_executable(feature_extractor feature_extractor.cpp)

# Link libraries
target_link_libraries(feature_extractor ${ONNXRUNTIME_LIBRARIES})

# Compiler flags
if(ONNXRUNTIME_CFLAGS_OTHER)
    target_compile_options(feature_extractor PRIVATE ${ONNXRUNTIME_CFLAGS_OTHER})
endif()

# Copy stb_image.h if not present
if(NOT EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/stb_image.h")
    file(DOWNLOAD 
        "https://raw.githubusercontent.com/nothings/stb/master/stb_image.h"
        "${CMAKE_CURRENT_SOURCE_DIR}/stb_image.h"
    )
endif() 