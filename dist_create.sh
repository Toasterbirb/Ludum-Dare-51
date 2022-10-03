#!/bin/sh

# Create a distributable .zip file with dependencies

echo "Removing existing build binaries"
[ -d ./build ] && rm -rv ./build
mkdir ./build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release -DSTATIC_SDL=On
make -j$(nproc)

mkdir diamond-heist

# Move the binary and resources
cp ./diamond_heist ./diamond-heist/
cp -r ./res ./diamond-heist/

zip -r ./diamond-heist.zip ./diamond-heist/
