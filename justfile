build:
    cmake -B build -DCMAKE_BUILD_TYPE=Debug -DCMAKE_EXPORT_COMPILE_COMMANDS=ON
    cmake --build build/
    ln -sf build/compile_commands.json .

profile: build
    perf record -g ./build/main

run: build
    ./build/main

debug: build
    gdb ./build/main

clean:
    rm -rf build/
