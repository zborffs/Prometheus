FROM gcc

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
    pip3 install gcovr
