#!/bin/bash

PLUGIN_NAME="EXAMPLE"
MODE="RelWithDebInfo"

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

# Determine the best available CMake generator

if command -v ninja &> /dev/null; then
    GENERATOR="-G Ninja"
    USE_NINJA=true
else
    GENERATOR=""  # Use CMake's default generator
    USE_NINJA=false
fi

MAIN_DIR=$(pwd)
BUILD_DIR="${MAIN_DIR}/build/${MODE}"

# Get the number of CPU cores
OS="$(uname)"
if [[ "$OS" == "Darwin" ]]; then
    CORES=$(sysctl -n hw.ncpu)  # macOS
elif [[ "$OS" == "Linux" ]] || [[ "$OS" == "CYGWIN"* ]] || [[ "$OS" == "MINGW"* ]]; then
    CORES=$(nproc)  # Linux, WSL, Git Bash, Cygwin, MinGW
else
    CORES=8  # Default to 8 if unknown OS (failsafe)
fi

echo "=================
PLUGIN: ${PLUGIN_NAME}
MODE: ${MODE}
ARCHITECTURE: ${ARCH}
GENERATOR: ${GENERATOR}
CORES: ${CORES}
================"
START_TIME=$(date +%s)

# Only run if no build folder or new CMake configuration
if [ ! -d "${BUILD_DIR}/CMakeFiles" ]; then
    mkdir -p "$BUILD_DIR"
    cd "$BUILD_DIR"

    if [ $ARCH = 'x86_64' ]; then
        cmake $GENERATOR -DCMAKE_OSX_ARCHITECTURES="x86_64" -DCMAKE_BUILD_TYPE="$MODE" -DCMAKE_CXX_FLAGS="-fcolor-diagnostics" "$MAIN_DIR"
    elif [ $ARCH = 'arm64' ]; then
        cmake $GENERATOR -DCMAKE_OSX_ARCHITECTURES="arm64;x86_64" -DCMAKE_BUILD_TYPE="$MODE" -DCMAKE_OSX_DEPLOYMENT_TARGET=10.15 -DCMAKE_CXX_FLAGS="-fcolor-diagnostics" "$MAIN_DIR"
    fi
    cd "$MAIN_DIR"
fi

# use max cores for default generator (ninja ignores this)
cmake --build "$BUILD_DIR" -j"$CORES" --config "$MODE"


END_TIME=$(date +%s)
TIME_ELAPSED=$(echo "$END_TIME - $START_TIME" | bc)

result=$?

if [ ${result} = 0 ]; then
  say "Build successful"
  echo "Elapsed time: ${TIME_ELAPSED} seconds"

else
  say "Build failed"
fi