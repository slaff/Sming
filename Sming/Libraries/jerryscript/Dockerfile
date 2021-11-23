FROM ubuntu:16.04

RUN apt-get update -q
RUN apt-get -q -y install \
    cmake \
    default-jre \
    git-core \
    python \
    python-pip \
    sudo

RUN pip install pylint==1.6.5

ENV SDK_VERSION latest

# [Activate last working version ]
RUN git clone https://github.com/emscripten-core/emsdk.git && \
    cd emsdk && \
    ./emsdk install $SDK_VERSION && \
    ./emsdk activate $SDK_VERSION && \
    /bin/bash -c "source ./emsdk_env.sh"

SHELL ["/bin/bash", "-c"]

RUN git clone -b feature/emscripten-jerryscript-v2 https://github.com/attachix/jerryscript

WORKDIR /jerryscript

ENV EMSCRIPTEN /emsdk/upstream/emscripten

RUN source /emsdk/emsdk_env.sh && ./tools/build.py --emscripten-snapshot-compiler ON --verbose --profile=minimal

# Install Clang-9
ENV LLVM_VERSION 9
RUN echo "deb http://apt.llvm.org/xenial/   llvm-toolchain-xenial-9  main" > /etc/apt/sources.list.d/llvm.list && \
    apt-get install -y wget && \
    wget -O - https://apt.llvm.org/llvm-snapshot.gpg.key | apt-key add - && \
    apt-get update && \
    apt-get install -y clang-$LLVM_VERSION lldb-$LLVM_VERSION lld-$LLVM_VERSION clangd-$LLVM_VERSION && \
    update-alternatives --install /usr/bin/clang clang /usr/bin/clang-9 1 && \
    update-alternatives --install /usr/bin/clang++ clang++ /usr/bin/clang++-9 1 && \
    update-alternatives --install /usr/bin/wasm-ld wasm-ld /usr/bin/wasm-ld-9 1 && \
    update-alternatives --install /usr/bin/llc llc-9 /usr/bin/llc-9 1

# Installing WebAssembly Toolkit
RUN cd / && \
     git clone --recursive https://github.com/WebAssembly/wabt && \
     make -C wabt -j$(nproc)

ENV PATH=$PATH:/wabt/bin

ENTRYPOINT source /emsdk/emsdk_env.sh && /bin/bash
CMD ["node","/jerryscript/build/bin/jsc.js"]
