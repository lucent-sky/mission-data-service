# =========================
# Stage 1: Build
# =========================
FROM ubuntu:22.04 AS builder

# Prevent interactive prompts
ENV DEBIAN_FRONTEND=noninteractive

# Install build dependencies
RUN apt-get update && apt-get install -y \
    build-essential \
    cmake \
    g++ \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /app

# Copy entire project
COPY . .

# Build
RUN mkdir build && cd build && \
    cmake -DCMAKE_BUILD_TYPE=Release .. && \
    cmake --build . --config Release

# =========================
# Stage 2: Runtime
# =========================
FROM ubuntu:22.04 AS runtime

ENV DEBIAN_FRONTEND=noninteractive

# Install minimal runtime dependencies
RUN apt-get update && apt-get install -y \
    libstdc++6 \
    && rm -rf /var/lib/apt/lists/*

# Create non-root user
RUN useradd -m -s /bin/bash appuser

WORKDIR /home/appuser

# Copy compiled binary from builder
COPY --from=builder /app/build/mission_data_service ./mission_data_service

# Change ownership
RUN chown appuser:appuser mission_data_service

USER appuser

# Default port (can override)
ENV PORT=8080

EXPOSE 8080

# Run application
CMD ["./mission_data_service"]
