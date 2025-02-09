#!/bin/bash

python src/parameters/create_parameters.py

PLUGIN_NAME="EXAMPLE"
MODE="RelWithDebInfo"

if ! [ -d "build" ]; then
  `mkdir -p build`
fi

# "./build.sh -m Debug", for example
while getopts "m:" flag
do
    case "${flag}" in
        m) MODE=${OPTARG};;
        *)
            echo "Invalid option: -${flag}" >&2
            exit 1
            ;;
    esac
done

ARCH=$(uname -m)

echo "=================
PLUGIN: ${PLUGIN_NAME}
MODE: ${MODE}
ARCHITECTURE: ${ARCH}
================"

cd build
if [ $ARCH = 'x86_64' ]; then
  cmake -DCMAKE_OSX_ARCHITECTURES="x86_64" -DCMAKE_BUILD_TYPE="$MODE" ..
elif [ $ARCH = 'arm64' ]; then
  cmake -DCMAKE_OSX_ARCHITECTURES="arm64;x86_64" -DCMAKE_BUILD_TYPE="$MODE" -DCMAKE_OSX_DEPLOYMENT_TARGET=10.15 ..
fi

cd ..
cmake --build build -j8 --config ${MODE}

result=$?

if [ ${result} == 0 ]; then

  say "Build successful"

  # CHANGE THIS LINE TO OPEN A ... DAW, or a debugger if you want

  # build/"$PLUGIN_NAME"_artefacts/"$MODE"/Standalone/"$PLUGIN_NAME".app/Contents/MacOS/"$PLUGIN_NAME"
  # lldb /Applications/REAPER.app/Contents/MacOS/REAPER peripheral/Test.rpp
  # /Applications/Ableton\ Live\ 11\ Suite.app/Contents/MacOS/Live peripheral/Test\ Project/Test.als

else
  say "Build failed"
fi