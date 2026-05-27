FROM ubuntu:24.04
RUN \
    dpkg --add-architecture i386 && \
    apt-get update && \
    apt-get install -y \
        cmake \
        g++ \
        g++-multilib \
        gcc \
        gcc-multilib \
        git \
        libc6:i386 \
        libstdc++6:i386 \
        ninja-build \
    && rm -rf /var/lib/apt/lists/*

COPY docker-entrypoint.sh /
CMD ["/docker-entrypoint.sh"]
