# Use CUDA-enabled base image for GPU support
FROM nvidia/cuda:11.8.0-runtime-ubuntu22.04

# Install Python and required packages
RUN apt-get update && apt-get install -y \
    python3-pip \
    python3-dev \
    libgl1-mesa-glx \
    xvfb \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /app

# Copy requirements first to leverage Docker cache
COPY ai_source/requirements.txt .
RUN pip3 install --no-cache-dir -r requirements.txt

# Copy AI training scripts, config, and executable
COPY ai_source/ ./ai_source/
COPY executable/ ./executable/

# Set up virtual display
ENV DISPLAY=:99

# Start Xvfb and run training
CMD Xvfb :99 -screen 0 1024x768x16 & python3 ai_source/train.py
