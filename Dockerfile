# Use Ubuntu 20.04 as base image
FROM ubuntu:20.04

# Set environment variables
ENV DEBIAN_FRONTEND=noninteractive
ENV TZ=UTC

# Install system dependencies
RUN apt-get update && apt-get install -y \
    build-essential \
    cmake \
    pkg-config \
    libssl-dev \
    curl \
    wget \
    git \
    && rm -rf /var/lib/apt/lists/*

# Install Node.js 18.x
RUN curl -fsSL https://deb.nodesource.com/setup_18.x | bash - \
    && apt-get install -y nodejs

# Install ONNX Runtime
RUN wget https://github.com/microsoft/onnxruntime/releases/download/v1.15.1/onnxruntime-linux-x64-1.15.1.tgz \
    && tar -xzf onnxruntime-linux-x64-1.15.1.tgz \
    && mv onnxruntime-linux-x64-1.15.1 /opt/onnxruntime \
    && rm onnxruntime-linux-x64-1.15.1.tgz

# Set ONNX Runtime environment variables
ENV ONNXRUNTIME_ROOT=/opt/onnxruntime
ENV LD_LIBRARY_PATH=/opt/onnxruntime/lib:$LD_LIBRARY_PATH
ENV PKG_CONFIG_PATH=/opt/onnxruntime/lib/pkgconfig:$PKG_CONFIG_PATH

# Create app directory and set as working directory
WORKDIR /opt/render/project/src

# Copy package files first for better caching
COPY package*.json ./

# Install Node.js dependencies
RUN npm ci --only=production

# Copy source code
COPY . .

# Download stb_image.h if not present
RUN if [ ! -f "stb_image.h" ]; then \
        wget -O stb_image.h https://raw.githubusercontent.com/nothings/stb/master/stb_image.h; \
    fi

# Build the C++ feature extractor
RUN mkdir -p build && cd build \
    && cmake .. \
    && make -j$(nproc) \
    && cp feature_extractor .. \
    && cd .. \
    && rm -rf build

# Create static directory if it doesn't exist
RUN mkdir -p static

# Expose port
EXPOSE 3000

# Start the application
CMD ["npm", "start"] 