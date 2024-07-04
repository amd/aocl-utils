# Use an official Ubuntu as a parent image
FROM ubuntu:24.04

# Use build arguments for Ubuntu version and GCC version
ARG C_COMPILER=gcc-12
ARG CXX_COMPILER=g++-12

RUN echo "Ubuntu version: $UBUNTU_VERSION"
RUN echo "C compiler: $C_COMPILER"
RUN echo "C++ compiler: $CXX_COMPILER"

# Set environment variables to avoid prompts during package installations
ENV DEBIAN_FRONTEND=noninteractive

# Update and install necessary packages
RUN apt-get update && apt-get install -y \
    build-essential \
    qemu-user \
    $C_COMPILER \
    $CXX_COMPILER \
    python3 \
    python3-dev \
    git \
    wget \
    && rm -rf /var/lib/apt/lists/*

# Set the working directory inside the container
VOLUME /usr/src/app
WORKDIR /usr/src/app


# Install CMake and build the project
# check if cmake folder exists
# if not, download and extract the cmake tarball
CMD sh Tests/build_sanity_test.sh  $CMAKE_VERSION $C_COMPILER $CXX_COMPILER
# List the directories and files in the current directory
# docker run -e CMAKE_VERSION=3.22 -e C_COMPILER=gcc-10 -e CXX_COMPILER=g++-10 -t utils_build_sanity_test
