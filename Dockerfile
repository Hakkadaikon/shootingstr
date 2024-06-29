FROM ubuntu:24.04

# Install related packages
RUN --mount=type=cache,target=/var/lib/apt/lists \
    --mount=type=cache,target=/var/cache/apt \
    apt-get -y update && \
    apt-get -y upgrade && \
    apt-get -y install --no-install-recommends make wget zip unzip git libc6-dev dpkg-dev libssl-dev openssl && \
    apt-get -y purge cmake

# Set default gcc g++
ARG CC "gcc-14"
ARG CXX "g++-14"
RUN --mount=type=cache,target=/var/lib/apt/lists \
    --mount=type=cache,target=/var/cache/apt \
    apt-get -y install --no-install-recommends ${CC} ${CXX}&& \
    gcc --version && \
    g++ --version

#ARG CMAKE_PKG_URL=https://cmake.org/files/v3.18/cmake-3.18.4-Linux-x86_64.sh
# If use openssl-cmake, you must use an older version of cmake to build.
ARG CMAKE_PKG_URL=https://cmake.org/files/v3.30/cmake-3.30.0-rc4-linux-x86_64.sh
RUN wget ${CMAKE_PKG_URL} && \
    sh $(basename ${CMAKE_PKG_URL}) --prefix=/usr/local --exclude-subdir && \
    rm $(basename ${CMAKE_PKG_URL})

# Install ninja
ARG NINJA_PKG_URL=http://archive.ubuntu.com/ubuntu/ubuntu/pool/universe/n/ninja-build/ninja-build_1.12.1-1_amd64.deb
RUN wget ${NINJA_PKG_URL} && \
    dpkg -i $(basename ${NINJA_PKG_URL}) && \
    rm $(basename ${NINJA_PKG_URL})

# Install meson
RUN --mount=type=cache,target=/var/lib/apt/lists \
    --mount=type=cache,target=/var/cache/apt \
    apt-get -y install --no-install-recommends python3 python3-pkg-resources python3-setuptools
ARG MESON_PKG_URL=http://archive.ubuntu.com/ubuntu/pool/universe/m/meson/meson_1.4.1-1ubuntu1_all.deb
RUN wget ${MESON_PKG_URL} && \
    dpkg -i $(basename ${MESON_PKG_URL}) && \
    rm $(basename ${MESON_PKG_URL})

WORKDIR /app

# Copy source files
COPY ./src      /app/src
COPY ./meson    /app/meson
COPY ./Makefile /app/Makefile

# Build
RUN make setup && make build

# Start shootingstr
RUN echo "start!"
CMD ["build/shootingstr"]
