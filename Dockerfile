# Start from Ubuntu 22.04
FROM ubuntu:latest

# Install required tools
RUN apt-get update && apt-get install -y \
    curl \
    git \
    nodejs \
    npm \
    docker.io \
    python3 \
    python3-pip \
    jq \
    build-essential \
    && rm -rf /var/lib/apt/lists/*

# Set the working directory (optional)
WORKDIR /github/workspace

# Set entrypoint (optional)
ENTRYPOINT ["/bin/bash"]