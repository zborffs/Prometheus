# Base is gcc to duplicate .gitlab-ci functionality
FROM gcc

# Download and install cmake, make, vim, python3, pip3, GTest, spdlog, lcov, and gcovr
RUN apt-get -y update && \
    apt-get install -y git cmake make vim python3 python3-pip && \
    mkdir -p /opt && cd /opt && \
    git clone https://github.com/google/googletest.git && \
    cd googletest && \
    cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=/usr/local . && \
    make -j && make install && \
    cd /opt && \
    git clone https://github.com/gabime/spdlog.git && \
    cd spdlog && \
    mkdir build && \
    cd build && \
    cmake .. && \
    make -j && \
    make install && \
    git clone https://github.com/linux-test-project/lcov.git && \
    make -C lcov install && \
    pip3 install gcovr && \
    cd /root && \
    apt-get clean && \
    rm -rf /var/lib/apt/lists/* /tmp/* /var/tmp/*

CMD ["/bin/bash"]

# Usually, I clone this repository (newest master) and build