#!/bin/sh

BUILD_DIR="./build"

# Delete any existing build directories
[ -d "$BUILD_DIR" ] && rm -rv "$BUILD_DIR"

# Create a fresh build directory and move into it
mkdir -pv "$BUILD_DIR"
cd "$BUILD_DIR"

cmake .. -DRELEASE=on -DWINDOWS=on -DTESTS=off -DUTILS=off -DSTATIC_SDL=on -DCMAKE_SYSTEM_NAME=Windows -DCMAKE_C_COMPILER=x86_64-w64-mingw32-gcc -DCMAKE_CXX_COMPILER=x86_64-w64-mingw32-g++ -DCMAKE_C_FLAGS="-fstack-protector -static" -DCMAKE_CXX_FLAGS="-fstack-protector -static -static-libstdc++" -DMINGW=TRUE

# Compile
make -j$(nproc)

# Move the binary and resources
mkdir diamond-heist-win
cp ./diamond_heist.exe ./diamond-heist-win/
cp -r ./res ./diamond-heist-win/

zip -r ./diamond-heist.zip ./diamond-heist-win/
