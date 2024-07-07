FROM ubuntu:24.04 AS relay

ENV DEBIAN_FRONTEND noninteractive

ARG GCC=gcc-14
ARG GPP=g++-14
ARG CMAKE_PKG_URL=https://cmake.org/files/v3.30/cmake-3.30.0-rc4-linux-x86_64.sh
ARG NINJA_PKG_URL=http://archive.ubuntu.com/ubuntu/ubuntu/pool/universe/n/ninja-build/ninja-build_1.12.1-1_amd64.deb
ARG MESON_PKG_URL=http://archive.ubuntu.com/ubuntu/pool/universe/m/meson/meson_1.4.1-1ubuntu1_all.deb

# If use openssl-cmake, you must use an older version of cmake to build.
#ARG CMAKE_PKG_URL=https://cmake.org/files/v3.18/cmake-3.18.4-Linux-x86_64.sh

# Install related packages
RUN \
  --mount=type=cache,target=/var/lib/apt/lists \
  --mount=type=cache,target=/var/cache/apt \
  apt-get -y update && \
  apt-get -y install --no-install-recommends \
    make \
    wget \
    zip \
    unzip \
    git \
    libc6-dev \
    dpkg-dev \
    ca-certificates \
    libssl-dev \
    openssl \
    libwebsockets-dev \
    ${GCC} \
    ${GPP} \
    python3 \
    python3-pkg-resources \
    python3-setuptools \
    && \
  update-alternatives --install /usr/bin/gcc gcc /usr/bin/${GCC} 99 && \
  update-alternatives --install /usr/bin/g++ g++ /usr/bin/${GPP} 99 && \
  update-alternatives --auto gcc && \
  update-alternatives --auto g++ && \
  wget ${CMAKE_PKG_URL} && \
  wget ${NINJA_PKG_URL} && \
  wget ${MESON_PKG_URL} && \
  sh $(basename ${CMAKE_PKG_URL}) --prefix=/usr/local --exclude-subdir && \
  dpkg -i $(basename ${NINJA_PKG_URL}) && \
  dpkg -i $(basename ${MESON_PKG_URL}) && \
  rm $(basename ${CMAKE_PKG_URL}) && \
  rm $(basename ${NINJA_PKG_URL}) && \
  rm $(basename ${MESON_PKG_URL})

# Copy source files
COPY ./src      /app/src
COPY ./meson    /app/meson
COPY ./Makefile /app/Makefile

WORKDIR /app

# Build
RUN make setup && make build

# Start shootingstr
RUN echo "start!"
CMD ["build/shootingstr"]
