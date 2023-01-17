#!/bin/bash

python src/parameters/create_parameters.py

if ! [ -d "build" ]; then
  `mkdir -p build`
fi

Architecture=$(uname -m)

cd build
if [ $Architecture = 'x86_64' ]; then
  cmake -DCMAKE_OSX_ARCHITECTURES="x86_64" -DCMAKE_BUILD_TYPE=Debug -DCMAKE_OSX_DEPLOYMENT_TARGET=10.10 ..
elif [ $Architecture = 'arm64' ]; then
  # you can build in debug mode, release mode, or release with debug symbols (useful for profiling performance)

  cmake -DCMAKE_OSX_ARCHITECTURES="arm64;x86_64" -DCMAKE_BUILD_TYPE=Debug -DCMAKE_OSX_DEPLOYMENT_TARGET=10.10 ..
  # cmake -DCMAKE_OSX_ARCHITECTURES="arm64;x86_64" -DCMAKE_BUILD_TYPE=RelWithDebInfo -DCMAKE_OSX_DEPLOYMENT_TARGET=10.10 ..
  # cmake -DCMAKE_OSX_ARCHITECTURES="arm64;x86_64" -DCMAKE_BUILD_TYPE=Release -DCMAKE_OSX_DEPLOYMENT_TARGET=10.10 ..
fi

cd ..
cmake --build build

result=$?

if [ ${result} == 0 ]; then

  say "Build successful"

  # After successful build you can use a command like one of these to launch the app or a debugger

  # if [ $Architecture = 'x86_64' ]; then
    # /Applications/Ableton\ Live\ 10\ Suite.app/Contents/MacOS/Live peripheral/demo\ Project/demo.als
    # lldb /Applications/REAPER.app/Contents/MacOS/REAPER #peripheral/demo/demo.RPP
    # lldb build/EXAMPLE_artefacts/Debug/Standalone/EXAMPLE.app/Contents/MacOS/EXAMPLE
  # elif [ $Architecture = 'arm64' ]; then
    # lldb /Applications/REAPER.app/Contents/MacOS/REAPER peripheral/Test.rpp
    # /Applications/Ableton\ Live\ 11\ Suite.app/Contents/MacOS/Live peripheral/Test\ Project/Test.als
    # lldb build/EXAMPLE_artefacts/Debug/Standalone/EXAMPLE.app/Contents/MacOS/EXAMPLE
  # fi
else
  say "Build failed"
fi