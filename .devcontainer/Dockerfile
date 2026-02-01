# Base image Ubuntu 24.04 with GCC 14 support
FROM ubuntu:24.04

# Image metadata
LABEL description="Devcontainer for FSMConfig C++ project (C++26)"
LABEL version="1.0.0"
LABEL maintainer="FSMConfig Team"

# Disable interactive prompts during package installation
ARG DEBIAN_FRONTEND=noninteractive

# Update package list and install base dependencies in one layer for cache optimization
RUN apt-get update && apt-get install -y \
    # Compilers and build tools
    build-essential \
    gcc-14 \
    g++-14 \
    # Build system
    cmake \
    # Project dependencies
    libyaml-cpp-dev \
    libgtest-dev \
    # Development utilities
    pkg-config \
    git \
    vim \
    curl \
    wget \
    # Debugging and analysis tools
    gdb \
    valgrind \
    # Formatting and static analysis tools
    clang-format \
    clang-tidy \
    # Clear apt cache to reduce image size
    && apt-get clean \
    && rm -rf /var/lib/apt/lists/*

# Set GCC 14 as default compiler
RUN update-alternatives --install /usr/bin/gcc gcc /usr/bin/gcc-14 100 \
    && update-alternatives --install /usr/bin/g++ g++ /usr/bin/g++-14 100 \
    && update-alternatives --set gcc /usr/bin/gcc-14 \
    && update-alternatives --set g++ /usr/bin/g++-14

# Create vscode user with UID 1000
RUN groupadd -g 1000 vscode \
    && useradd -u 1000 -g vscode -m -s /bin/bash vscode

# Add user to sudoers without password
RUN echo "vscode ALL=(ALL) NOPASSWD:ALL" > /etc/sudoers.d/vscode \
    && chmod 0440 /etc/sudoers.d/vscode

# Create working directory and set permissions
RUN mkdir -p /workspace \
    && chown -R vscode:vscode /workspace

# Switch to vscode user
USER vscode

# Set working directory
WORKDIR /workspace

# Set environment variable for C++26
ENV CXXFLAGS="-std=c++26"

# Default command when container starts
CMD ["/bin/bash"]
