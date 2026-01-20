# Optimized Docker image for FSMConfig CI/CD
FROM ubuntu:24.04

LABEL description="FSMConfig CI Image"
LABEL version="1.0.0"
LABEL usage="Optimized for CI/CD pipelines"

# Set working directory
WORKDIR /app

# Set environment variables for C++26 and Release build
ENV CXXFLAGS="-std=c++26"
ENV CMAKE_BUILD_TYPE=Release

# Install minimal set of packages in a single layer to reduce image size
RUN apt-get update && \
    apt-get install -y --no-install-recommends \
        build-essential \
        cmake \
        libyaml-cpp-dev \
        libgtest-dev \
        pkg-config \
        git && \
    # Set GCC 14 as default compiler
    update-alternatives --install /usr/bin/gcc gcc /usr/bin/gcc-14 100 && \
    update-alternatives --install /usr/bin/g++ g++ /usr/bin/g++-14 100 && \
    # Clean up to reduce image size
    apt-get autoremove -y && \
    apt-get clean && \
    rm -rf /var/lib/apt/lists/* /tmp/* /var/tmp/*
