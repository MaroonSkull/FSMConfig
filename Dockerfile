# Optimized Docker image for FSMConfig CI/CD
FROM ubuntu:24.04

LABEL description="FSMConfig CI Image"
LABEL version="1.0.0"
LABEL usage="Optimized for CI/CD pipelines"

# Set working directory
WORKDIR /app

# Set environment variables for C++20 and Release build
ENV CXXFLAGS="-std=c++20"
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
    # Clean up to reduce image size
    apt-get autoremove -y && \
    apt-get clean && \
    rm -rf /var/lib/apt/lists/* /tmp/* /var/tmp/*
