# Example Plugin CMAKE file
# Nathan Blair May 2023 - modified from the example CMAKE file from JUCE

set(ENV{PLUGIN_NAME} EXAMPLE)
set(ENV{VERSION_NUMBER} 0x000001) # 0xVVMMPP (VV = version, MM = modification, PP = patch)
set(ENV{VERSION_STRING} 0.0.1) # VV.MM.PP
set(ENV{COMPANY_NAME} YOUR_COMPANY_NAME)
set(ENV{MANUFACTURER_ID} MFID) # A four-character manufacturer id with at least one upper-case character
set(ENV{PLUGIN_CODE} Plid) # A unique four-character plugin id with exactly one upper-case character


#--------------------------------------------------------------------------------
# The first line of any CMake project should be a call to `cmake_minimum_required`, which checks
# that the installed CMake will be able to understand the following CMakeLists, and ensures that
# CMake's behaviour is compatible with the named version. This is a standard CMake command, so more
# information can be found in the CMake docs.
#--------------------------------------------------------------------------------

cmake_minimum_required(VERSION 3.15)

#--------------------------------------------------------------------------------
# The top-level CMakeLists.txt file for a project must contain a literal, direct call to the
# `project()` command. `project()` sets up some helpful variables that describe source/binary
# directories, and the current project version. This is a standard CMake command.
#--------------------------------------------------------------------------------

project($ENV{PLUGIN_NAME} VERSION $ENV{VERSION_STRING})


#--------------------------------------------------------------------------------
# Include JUCE (as a subdirectory)
# It is recommended to have a separate JUCE subdirectory for every project you create. 
# This prevents issues that might arise from different versions of JUCE being used in different projects.
#--------------------------------------------------------------------------------

add_subdirectory(JUCE) # If you've put JUCE in a subdirectory

#--------------------------------------------------------------------------------
# If you are building a VST2 or AAX plugin, CMake needs to be told where to find these SDKs on your
# system. This setup should be done before calling `juce_add_plugin`.
#--------------------------------------------------------------------------------

# juce_set_vst2_sdk_path(...)
# juce_set_aax_sdk_path(...)

#--------------------------------------------------------------------------------
# `juce_add_plugin` adds a static library target with the name passed as the first argument
# (AudioPluginExample here). This target is a normal CMake target, but has a lot of extra properties set
# up by default. As well as this shared code static library, this function adds targets for each of
# the formats specified by the FORMATS arguments. This function accepts many optional arguments.
# Check the readme at `docs/CMake API.md` in the JUCE repo for the full list.
# SEE: https://github.com/juce-framework/JUCE/blob/master/docs/CMake%20API.md
#--------------------------------------------------------------------------------

juce_add_plugin($ENV{PLUGIN_NAME}
    # VERSION ...                               # Set this if the plugin version is different to the project version
    # ICON_BIG ...                              # ICON_* arguments specify a path to an image file to use as an icon for the Standalone
    # ICON_SMALL ...
    COMPANY_NAME $ENV{COMPANY_NAME}       # Specify the name of the plugin's author
    IS_SYNTH FALSE                       # Is this a synth or an effect?
    NEEDS_MIDI_INPUT FALSE               # Does the plugin need midi input?
    NEEDS_MIDI_OUTPUT FALSE              # Does the plugin need midi output?
    IS_MIDI_EFFECT FALSE                 # Is this plugin a MIDI effect?
    EDITOR_WANTS_KEYBOARD_FOCUS TRUE    # Does the editor need keyboard focus? (do you need to use the keyboard for this plugin?)
    COPY_PLUGIN_AFTER_BUILD TRUE        # Should the plugin be installed to a default location after building?
    PLUGIN_MANUFACTURER_CODE $ENV{MANUFACTURER_ID}               # A four-character manufacturer id with at least one upper-case character
    PLUGIN_CODE $ENV{PLUGIN_CODE}                            # A unique four-character plugin id with exactly one upper-case character
    HARDENED_RUNTIME_ENABLED TRUE               # Important for distributing with an installer on MacOS
    FORMATS AU VST3 Standalone                  # The formats to build. Other valid formats are: AAX Unity VST AU AUv3
    PRODUCT_NAME $ENV{PLUGIN_NAME})        # The name of the final executable, which can differ from the target name

#--------------------------------------------------------------------------------
# `juce_generate_juce_header` will create a JuceHeader.h for a given target, which will be generated
# into your build tree. This should be included with `#include <JuceHeader.h>`. The include path for
# this header will be automatically added to the target. The main function of the JuceHeader is to
# include all your JUCE module headers; if you're happy to include module headers directly, you
# probably don't need to call this.
#--------------------------------------------------------------------------------

# juce_generate_juce_header($ENV{PLUGIN_NAME})
set(VERSION_NUMBER $ENV{VERSION_NUMBER})
set(VERSION_STRING $ENV{VERSION_STRING})
set(PROJECT_NAME $ENV{PLUGIN_NAME})
set(COMPANY_NAME $ENV{COMPANY_NAME})
configure_file(src/plugin/ProjectInfo.cpp.in ${PROJECT_SOURCE_DIR}/src/plugin/ProjectInfo.cpp @ONLY)

#--------------------------------------------------------------------------------
# Add source files (Only include .cpp files here, not headers)
#--------------------------------------------------------------------------------

target_sources($ENV{PLUGIN_NAME}
    PRIVATE
        src/plugin/ProjectInfo.cpp
        src/plugin/PluginProcessorBase.cpp
        src/plugin/PluginProcessor.cpp
        src/plugin/PluginEditor.cpp
        src/parameters/StateManager.cpp
        src/interface/ParameterSlider.cpp
        src/audio/Gain.cpp
        )

#--------------------------------------------------------------------------------
# I prefer to use c++ 17, but you can also use a different c++ version
target_compile_features($ENV{PLUGIN_NAME} PRIVATE cxx_std_17) # -std=c++17
#--------------------------------------------------------------------------------

#--------------------------------------------------------------------------------
# Add preprocessor definitions
# SEE: https://github.com/juce-framework/JUCE/blob/master/docs/CMake%20API.md
#--------------------------------------------------------------------------------

target_compile_definitions($ENV{PLUGIN_NAME}
    PUBLIC
        # JUCE_WEB_BROWSER and JUCE_USE_CURL would be on by default, but you might not need them.
        JUCE_WEB_BROWSER=0  # If you remove this, add `NEEDS_WEB_BROWSER TRUE` to the `juce_add_plugin` call
        JUCE_USE_CURL=0     # If you remove this, add `NEEDS_CURL TRUE` to the `juce_add_plugin` call
        JUCE_VST3_CAN_REPLACE_VST2=0
        NEEDS_SIDECHAIN=0
        JUCE_DONT_ASSERT_ON_GLSL_COMPILE_ERROR=1
        JUCE_MODAL_LOOPS_PERMITTED=1
)


#--------------------------------------------------------------------------------
# Include static resources like images, sounds, etc.
# Reference in code: BinaryData::YOUR_IMAGE_HERE_png, BinaryData::YOUR_IMAGE_HERE_png_Size
#--------------------------------------------------------------------------------

# juce_add_binary_data(AudioPluginData SOURCES 
#                         resources/YOUR_IMAGE_HERE.png
# )

#--------------------------------------------------------------------------------
# Add JUCE modules / any external libraries here
#--------------------------------------------------------------------------------

target_link_libraries($ENV{PLUGIN_NAME}
    PRIVATE
        # AudioPluginData           # If we'd created a binary data target, we'd link to it here
        juce::juce_audio_utils
    PUBLIC
        juce::juce_recommended_config_flags
        juce::juce_recommended_lto_flags
        juce::juce_recommended_warning_flags)


