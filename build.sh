#!/bin/bash

PLUGIN_NAME="EXAMPLE"
MODE="RelWithDebInfo"

# Process options (-m for mode).
while getopts "m:" flag; do
    case "${flag}" in
        m) MODE=${OPTARG} ;;
        *)
            echo "Invalid option: -${flag}" >&2
            exit 1
            ;;
    esac
done

ARCH=$(uname -m)

# Determine the best available CMake generator.
if command -v ninja &> /dev/null; then
    GENERATOR="-G Ninja"
    USE_NINJA=true
else
    GENERATOR=""  # Use CMake's default generator.
    USE_NINJA=false
fi

MAIN_DIR=$(pwd)
BUILD_DIR="${MAIN_DIR}/build/${MODE}"

# Get the number of CPU cores.
OS="$(uname)"
if [[ "$OS" == "Darwin" ]]; then
    CORES=$(sysctl -n hw.ncpu)
elif [[ "$OS" == MINGW* ]]; then
    CORES=${NUMBER_OF_PROCESSORS:-8}
else
    CORES=8  # Fallback (should not occur).
fi

echo "================="
echo "PLUGIN: ${PLUGIN_NAME}"
echo "MODE: ${MODE}"
echo "ARCHITECTURE: ${ARCH}"
echo "GENERATOR: ${GENERATOR}"
echo "CORES: ${CORES}"
echo "================="

START_TIME=$(date +%s)

# Configure CMake if not already configured.
if [ ! -d "${BUILD_DIR}/CMakeFiles" ]; then
    mkdir -p "$BUILD_DIR"
    cd "$BUILD_DIR"
    if [[ "$OS" == "Darwin" ]]; then
        if [ "$ARCH" = "x86_64" ]; then
            cmake $GENERATOR -DCMAKE_OSX_ARCHITECTURES="x86_64" -DCMAKE_BUILD_TYPE="$MODE" -DCMAKE_CXX_FLAGS="-fcolor-diagnostics" "$MAIN_DIR"
        elif [ "$ARCH" = "arm64" ]; then
            cmake $GENERATOR -DCMAKE_OSX_ARCHITECTURES="arm64;x86_64" -DCMAKE_BUILD_TYPE="$MODE" -DCMAKE_OSX_DEPLOYMENT_TARGET=10.15 -DCMAKE_CXX_FLAGS="-fcolor-diagnostics" "$MAIN_DIR"
        else
            cmake $GENERATOR -DCMAKE_BUILD_TYPE="$MODE" "$MAIN_DIR"
        fi
    elif [[ "$OS" == MINGW* ]]; then
        # Windows configuration (no macOS-specific flags).
        cmake $GENERATOR -DCMAKE_BUILD_TYPE="$MODE" "$MAIN_DIR"
    fi
    cd "$MAIN_DIR"
fi

# Build the project using the available cores.
cmake --build "$BUILD_DIR" -j"$CORES" --config "$MODE"

END_TIME=$(date +%s)
TIME_ELAPSED=$((END_TIME - START_TIME))

echo "Elapsed time: ${TIME_ELAPSED} seconds"
