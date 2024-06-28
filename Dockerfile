FROM ubuntu:24.04

WORKDIR /app

# Copy source files
COPY ./src      /app/src
COPY ./meson    /app/meson
COPY ./Makefile /app/Makefile

# Install related packages
RUN apt-get -y update
RUN apt-get -y upgrade
RUN apt-get -y install make wget zip unzip git build-essential
RUN apt-get -y install libssl-dev openssl

# Install cmake
RUN apt-get -y remove cmake
ENV CMAKE_PKG_URL=https://cmake.org/files/v3.30/cmake-3.30.0-rc4-linux-x86_64.sh
#ENV CMAKE_PKG_URL=https://cmake.org/files/v3.18/cmake-3.18.4-Linux-x86_64.sh
RUN wget ${CMAKE_PKG_URL}
RUN sh $(basename ${CMAKE_PKG_URL}) --prefix=/usr/local --exclude-subdir

# Install ninja
ENV NINJA_PKG_URL=http://archive.ubuntu.com/ubuntu/ubuntu/pool/universe/n/ninja-build/ninja-build_1.12.1-1_amd64.deb
RUN wget ${NINJA_PKG_URL}
RUN dpkg -i $(basename ${NINJA_PKG_URL})

# Install meson
RUN apt-get -y install python3 python3-pkg-resources python3-setuptools
ENV MESON_PKG_URL=http://archive.ubuntu.com/ubuntu/pool/universe/m/meson/meson_1.4.1-1ubuntu1_all.deb
RUN wget ${MESON_PKG_URL}
RUN dpkg -i $(basename ${MESON_PKG_URL})

# Set default gcc
ENV GCC=gcc-14
RUN apt-get -y install ${GCC} && \
  update-alternatives --install /usr/bin/gcc gcc /usr/bin/${GCC} 99 && \
  update-alternatives --auto gcc && \
  gcc --version

# Set default g++
ENV GPP=g++-14
RUN apt-get -y install ${GPP} && \
  update-alternatives --install /usr/bin/g++ g++ /usr/bin/${GPP} 99 && \
  update-alternatives --auto g++ && \
  g++ --version

# Build
RUN make setup
RUN make build

# Start shootingstr
RUN echo "start!"
CMD ["build/shootingstr"]
