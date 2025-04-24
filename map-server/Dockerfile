# map-server/Dockerfile
FROM debian:bookworm-slim

# Install dependencies
RUN apt-get update && apt-get install -y \
    build-essential \
    cmake \
    git \
    libboost-all-dev \
    libexpat1-dev \
    libfreetype6-dev \
    libgeos-dev \
    libproj-dev \
    libsqlite3-dev \
    libxml2-dev \
    pkg-config \
    python3-pip \
    python3-setuptools \
    python3-wheel \
    zlib1g-dev \
    && rm -rf /var/lib/apt/lists/*

# Create build directory
WORKDIR /usr/src/app

# Copy the files of the repository
COPY . .

# Compile in c++
RUN mkdir -p build && cd build && cmake .. && make

# default executable
WORKDIR /usr/src/app/build
CMD ["./udp_server", "--help"]