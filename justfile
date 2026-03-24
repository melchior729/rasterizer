run:
    ./build/main

clean:
    rm -rf build

build:
    cmake -B build -DCMAKE_BUILD_TYPE=Debug
    cmake --build build
