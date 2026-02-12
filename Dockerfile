# ==============================
# Stage 1: Build
# ==============================
FROM ubuntu:22.04 AS builder

# Install build dependencies
RUN apt-get update && apt-get install -y \
    build-essential \
    cmake \
    git \
    curl \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /app

# Copy project
COPY . .

# Create build directory
RUN cmake -S . -B build -DCMAKE_BUILD_TYPE=Release \
    && cmake --build build --config Release

# Run tests
RUN cmake -S . -B build -DCMAKE_BUILD_TYPE=Release \
    && cmake --build build \
    && cd build && ctest --output-on-failure

# ==============================
# Stage 2: Runtime
# ==============================
FROM ubuntu:22.04

# Create non-root user
RUN useradd -m missionuser

WORKDIR /app

# Copy binary from builder stage
COPY --from=builder /app/build/mission_data_service /app/mission_data_service

# Use non-root user
USER missionuser

EXPOSE 8080

ENTRYPOINT ["./mission_data_service"]
