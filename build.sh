#!/bin/bash

PLUGIN_NAME="EXAMPLE"
MODE="RelWithDebInfo"

# Process options (-m for mode).
while [[ $# -gt 0 ]]; do
    case $1 in
        -m|--mode)
            MODE="$2"
            shift 2
            ;;
        --ci)
            CI_MODE="ON"
            shift
            ;;
        *)
            echo "Unknown option: $1" >&2
            exit 1
            ;;
    esac
done

ARCH=$(uname -m)

# Determine the best available CMake generator.
if [[ "$OS" == "Darwin" ]] && command -v ninja &> /dev/null; then
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
echo "CI MODE: ${CI_MODE}"
echo "GENERATOR: ${GENERATOR}"
echo "CORES: ${CORES}"
echo "================="

# Set CI_BUILD flag for CMake if --ci was passed.
if [ "$CI_MODE" == "ON" ]; then
    CI_BUILD_FLAG="-DCI_BUILD=ON"
else
    CI_BUILD_FLAG=""
fi

START_TIME=$(date +%s)

# Configure CMake if not already configured.
if [ ! -d "${BUILD_DIR}/CMakeFiles" ]; then
    mkdir -p "$BUILD_DIR"
    cd "$BUILD_DIR"
    if [[ "$OS" == "Darwin" ]]; then
        if [ "$ARCH" = "x86_64" ]; then
            cmake $GENERATOR -DCMAKE_OSX_ARCHITECTURES="x86_64" -DCMAKE_BUILD_TYPE="$MODE" -DCMAKE_CXX_FLAGS="-fcolor-diagnostics" $CI_BUILD_FLAG "$MAIN_DIR"
        elif [ "$ARCH" = "arm64" ]; then
            cmake $GENERATOR -DCMAKE_OSX_ARCHITECTURES="arm64;x86_64" -DCMAKE_BUILD_TYPE="$MODE" -DCMAKE_OSX_DEPLOYMENT_TARGET=10.15 $CI_BUILD_FLAG -DCMAKE_CXX_FLAGS="-fcolor-diagnostics" "$MAIN_DIR"
        fi
    elif [[ "$OS" == MINGW* ]]; then
        # Windows configuration (no macOS-specific flags).
        cmake $GENERATOR -DCMAKE_BUILD_TYPE="$MODE" $CI_BUILD_FLAG "$MAIN_DIR"
    fi
    cd "$MAIN_DIR"
fi

# Build the project using the available cores.
cmake --build "$BUILD_DIR" -j"$CORES" --config "$MODE"

END_TIME=$(date +%s)
TIME_ELAPSED=$((END_TIME - START_TIME))

echo "Elapsed time: ${TIME_ELAPSED} seconds"
