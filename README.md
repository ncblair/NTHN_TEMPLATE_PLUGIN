**# Example JUCE Plugin for MAT 240B w/ Karl Yerkes**

a quick way to setup a JUCE plugin project



**Setup Directions**: 

```
git clone https://github.com/ncblair/MAT240B_JUCE_TEMPLATE.git
cd MAT240B_JUCE_TEMPLATE
git submodule update --recursive --init --remote
```
r


**Build Directions: (MacOS)**

```
./build.sh
```

Then, open the plugin called "EXAMPLE" in your DAW of choice or run the standalone app which can be found in  *build/EXAMPLE_artefacts/Debug/Standalone/EXAMPLE.app*

**Build Directions: (windows):**

```
mkdir build
cd build
cmake ..
cmake --build . (--config Release/Debug/...)
```

**Build Troubleshooting**

try this article for building with CMAKE: https://github.com/juce-framework/JUCE/blob/master/docs/CMake%20API.md



**How to modify the code**

- write audio code in src/plugin/PluginProcessor.cpp 
- write UI code in src/plugin/PluginEditor.cpp
- add parameters in src/parameters/parameters.csv
- link additional source files and resources in CMakeLists.txt