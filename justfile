build:
    cmake -B build -DCMAKE_BUILD_TYPE=Debug -DCMAKE_EXPORT_COMPILE_COMMANDS=ON
    cmake --build build/
    ln -sf build/compile_commands.json .

run: build
    ./build/main

debug: build
    gdb ./build/main

clean:
    rm -rf build/
