#!/bin/bash

python src/parameters/create_parameters.py
# python src/matrix/create_modulators.py
if ! [ -d "build" ]; then
  `mkdir -p build`
fi

Architecture=$(uname -m)

cd build
if [ $Architecture = 'x86_64' ]; then
  cmake -DCMAKE_OSX_ARCHITECTURES="x86_64" -DCMAKE_BUILD_TYPE=Debug ..
elif [ $Architecture = 'arm64' ]; then
  cmake -DCMAKE_OSX_ARCHITECTURES="arm64;x86_64" -DCMAKE_BUILD_TYPE=Debug -DCMAKE_OSX_DEPLOYMENT_TARGET=10.10 ..
  # cmake -DCMAKE_OSX_ARCHITECTURES="arm64;x86_64" -DCMAKE_BUILD_TYPE=RelWithDebInfo -DCMAKE_OSX_DEPLOYMENT_TARGET=10.10 ..
fi

cd ..
cmake --build build

result=$?

if [ ${result} == 0 ]; then

  say "Build successful"

  if [ $Architecture = 'x86_64' ]; then
    /Applications/Ableton\ Live\ 10\ Suite.app/Contents/MacOS/Live peripheral/demo\ Project/demo.als
    # lldb /Applications/REAPER.app/Contents/MacOS/REAPER #peripheral/demo/demo.RPP
    # lldb build/PEBBLE_artefacts/Debug/Standalone/PEBBLE.app/Contents/MacOS/PEBBLE
  elif [ $Architecture = 'arm64' ]; then
    # lldb /Applications/REAPER.app/Contents/MacOS/REAPER peripheral/Test.rpp
    /Applications/Ableton\ Live\ 11\ Suite.app/Contents/MacOS/Live peripheral/Test\ Project/Test.als
    # lldb build/PEBBLE_artefacts/Debug/Standalone/PEBBLE.app/Contents/MacOS/PEBBLE
  fi
else
  say "Build failed"
fi