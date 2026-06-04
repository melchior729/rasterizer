run: build
    ./build/main

run-web: web
  chromium 0.0.0.0:8000
  echo "Refresh when the port is served to access the site"
  python3 -m http.server --directory web/

build-all: build web

build:
    cmake -B build -DCMAKE_BUILD_TYPE=Debug -DCMAKE_EXPORT_COMPILE_COMMANDS=ON
    cmake --build build/
    ln -sf build/compile_commands.json .

web:
    if [ ! -f "web/rasterizer.js" ]; then \
    ./build-web.sh; \
    fi

debug: build
    gdb ./build/main
    
profile: build
    perf record -g ./build/main

clean:
    rm -rf build/
    rm -f web/rasterizer.js web/rasterizer.wasm

copy-cpp:
    mv ./include/stb_image.h .
    cat include/* src/* | wl-copy
    mv stb_image.h include/
