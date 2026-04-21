build:
    cmake -B build -DCMAKE_BUILD_TYPE=Debug
    cmake --build build/

run: build
    ./build/main

debug: build
    gdb ./build/main

clean:
    rm -rf build/
