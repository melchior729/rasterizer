run:
    just build
    just rerun

rerun:
    ./build/main

clean:
    rm -rf build

build:
    cmake -B build -DCMAKE_BUILD_TYPE=Debug
    cmake --build build

debug:
    gdb ./build/main
