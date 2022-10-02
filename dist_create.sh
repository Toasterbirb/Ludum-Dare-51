#!/bin/sh

# Create a distributable .zip file with dependencies

echo "Removing existing build binaries"
[ -d ./build ] && rm -rv ./build
mkdir ./build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j$(nproc)

mkdir diamond-heist
mkdir diamond-heist/libs

# Move the binary and resources
cp ./diamond_heist ./diamond-heist/
cp -r ./res ./diamond-heist/

# Copy in 64-bin libsdl from the host system
cp /usr/lib64/libSDL2-2.0.so.0.2400.0 ./diamond-heist/libs/libSDL2-2.0.so.0
echo "#!/bin/sh\nLD_LIBRARY_PATH=./libs/ ./diamond_heist" > ./diamond-heist/run_portable.sh
chmod +x ./diamond-heist/run_portable.sh

zip -r ./diamond-heist.zip ./diamond-heist/
