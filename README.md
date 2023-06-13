# The Template Plugin

The template plugin is a starting point for new JUCE plugin projects. The main contribution of the template plugin is the 'StateManager' class, which provides an API (Application Programming Interface) for real-time safe interaction with the state of the plugin between threads. Furthermore, I include real-time safe interface sliders that interact with the StateManager via polling. 

## Downloading the Template

To download The Template Plugin, clone the repository from GitHub. Update submodules to get a copy of the JUCE library in the same folder.

```sh
git clone https://github.com/ncblair/NTHN_TEMPLATE_PLUGIN.git
cd NTHN_TEMPLATE_PLUGIN
git submodule update --recursive --init --remote
```

Alternatively, if you would like to set up your own git repository with the code from The Template Plugin, you can create a new repository from a template using the github CLI tool `gh`.

```sh
# If this is your first time using gh, install it and log in
brew install gh
gh auth login
# create a (private) repository based on the template
gh repo create my_new_juce_proj --template ncblair/NTHN_TEMPLATE_PLUGIN --private
# clone the template repo locally
gh repo clone my_new_juce_proj
# update submodules to get JUCE code locally
cd my_new_juce_proj
git submodule update --recursive --init --remote
```

## Compiling the Template

First, open `CMakeLists.txt` in a text editor. Set your plugin name, version number, version string, company name, manufacturer id, and plugin id at the top of the file.

On MacOS, open `build.sh` in a text editor. Set the plugin name and build mode at the top of the file. Use the same plugin name that you used in `CMakeLists.txt`. Possible build modes include `Debug`, `Release`, `RelWithDebInfo` and `MinSizeRel`.

Then, run the build script
```sh
./build.sh
```

On Windows, run the following code:

```sh
mkdir build
cd build
cmake ..
cmake --build . # (--config Release/Debug/...)
```

## Running the Template Plugin

If compiling was successful, you should already be able to run the plugin in your DAW of choice. Simply open your DAW and search for your plugin name. By default, the plugin will be called EXAMPLE. 

To run the plugin as a standalone application, run the file found in  *build/EXAMPLE_artefacts/Debug/Standalone/EXAMPLE.app*

By default, you should see a green background with a single slider that modulates the gain of the incoming signal. 

## Editing Plugin Parameters in the Template Plugin

The template enables easy creation and modification of plugin parameters. The typical process for creating parameters in JUCE is bulky and requires many line of code for a single parameter:

```c++
std::vector<std::unique_ptr<juce::RangedAudioParameter>> params;
auto param_range = juce::NormalisableRange<float>(-60.0f, 6.0f, 0.0f, 1.0f);
auto param_suffix = "db";
auto param_default = 0.0f;
auto param_ID = "GAIN";
auto param_name = "Gain";
auto param = std::make_unique<juce::AudioParameterFloat>(
    juce::ParameterID{param_ID, ProjectInfo::versionNumber},   // parameter ID
    param_name,   // parameter name
    param_range,  // range
    param_default,// default value
    "", // parameter label (description?)
    juce::AudioProcessorParameter::Category::genericParameter,
    [p_id](float value, int maximumStringLength) { // Float to String Precision 2 Digits
        std::stringstream ss;
        ss << std::fixed << std::setprecision(0) << value;
        res = juce::String(ss.str());
        return (res + " " + param_suffix).substring(0, maximumStringLength);
    },
    [p_id](juce::String text) {
        text = text.upToFirstOccurrenceOf(" " + param_suffix, false, true);
        return text.getFloatValue(); // Convert Back to Value
    }
)
params.push_back(param);
apvts.reset(new juce::AudioProcessorValueTreeState(
    *processor, 
    &undo_manager, 
    juce::Identifier("PARAMETERS"), 
    {params.begin(), params.end()}
));
```

It's inconvenient to type all of this code every time you want to add a new plugin parameter. Instead, I set relevant parameter metadata in a csv file, `parameters.csv`. Adding a parameter becomes as simple as defining the relevant information in a table. 

PARAMETER | MIN | MAX | GRAIN | EXP | DEFAULT | AUTOMATABLE | NAME | SUFFIX | TOOLTIP | TO_STRING_ARR
--- | --- | --- | --- | --- | --- | --- | --- | --- | --- | ---
GAIN | -60 | 6 | 0 | 1 | 0 | 1 | Gain | db | The gain in decibels |

To convert between table data and JUCE parameters, a pre-build python script reads the `parameters.csv` file and generates c++ code that the StateManager class can use to create plugin parameters. This code is exported to the file `parameters/ParameterDefines.h` as a number of arrays of useful parameter information which can be accessed by the rest of the code. Any code that imports `parameters/StateManager.h` will also have access to the definitions in `ParameterDefines.h`. This c++ file also defines an enum which can be used to reference parameters. The following code shows how to access various attributes of a parameter from within the codebase:

```c++
#include "parameters/StateManager.h"
juce::Identifier parameter_ID = PARAMETER_IDS[PARAM::GAIN];
juce::String parameter_name = PARAMETER_NAMES[PARAM::GAIN];
juce::String display_name = PARAMETER_NICKNAMES[PARAM::GAIN];
juce::NormalisableRange<float> param_range = PARAMETER_RANGES[PARAM::GAIN];
float default_value = PARAMETER_DEFAULTS[PARAM::GAIN];
bool is_visible_to_host = PARAMETER_AUTOMATABLE[PARAM::GAIN];
juce::String parameter_suffix = PARAMETER_SUFFIXES[PARAM::GAIN];
juce::String tooltip = PARAMETER_TOOLTIPS[PARAM::GAIN];
// Given a parameter value, v, to_string_arr[v] is the string representation of the parameter value. 
// to_string_arr can be used to implement drop down menus.
// if to_string_arr is not defined, the vector will be empty.
int v = int(state->param_value(PARAM::TYPE));
juce::String string_repr_of_param = PARAMETER_TO_STRING_ARRS[PARAM::TYPE][v];
```

For more information about accessing the parameters of the plugin, reference `parameters/StateManager.h`.

## Editing Audio Code in the Template Plugin

Audio code in the template plugin belongs in the `PluginProcessor` class, which is defined in the files `PluginProcessor.cpp` and `PluginProcessor.h`. The `PluginProcessor::processBlock` is the main audio callback, invoked once per audio block. This function takes in a buffer of audio samples and a buffer of midi messages and fills the input buffer with the desired output samples. 

By default, the template plugin code applies a gain parameter to the audio buffer in the `processBlock`, shown below:

```c++
void PluginProcessor::processBlock (juce::AudioBuffer<float>& buffer,
                                              juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    // ...

    //--------------------------------------------------------------------------------
    // read in some parameter values here, if you want
    // in this case, gain goes from 0 to 100 (see: ../parameters/parameters.csv)
    // so we normalize it to 0 to 1
    //--------------------------------------------------------------------------------
    auto gain = state->param_value(PARAM::GAIN) / 100.0f;

    //--------------------------------------------------------------------------------
    // process samples below. use the buffer argument that is passed in.
    // for an audio effect, buffer is filled with input samples, and you should fill it with output samples
    // for a synth, buffer is filled with zeros, and you should fill it with output samples
    // see: https://docs.juce.com/master/classAudioBuffer.html
    //--------------------------------------------------------------------------------
    buffer.applyGain(gain);

    //--------------------------------------------------------------------------------
    // you can use midiMessages to read midi if you need. 
    // since we are not using midi yet, we clear the buffer.
    //--------------------------------------------------------------------------------
    midiMessages.clear();
}
```

To access plugin parameters from the process block, invoke the `StateManager::param_value` method by passing the enum of the desired parameter, as defined in `parameters/ParameterDefines.h` and equivalently in `parameters/parameters.csv`. 

## Editing Interface Code in the Template Plugin

To edit the user interface, define user interface components in `plugin/PluginEditor.h`. 

```c++
// PluginEditor.h, private:
private:
    // A single slider
    std::unique_ptr<ParameterSlider> gain_slider;
    // a second slider
    std::unique_ptr<ParameterSlider> parameter_2_slider;
```

Then, add, position, and trigger repaint calls for UI elements in `plugin/PluginEditor.cpp`. 

```c++
// Add elements in the constructor
AudioPluginAudioProcessorEditor::AudioPluginAudioProcessorEditor (PluginProcessor& p)
    : AudioProcessorEditor (&p), processorRef (p)
{
    ...

    // add slider BEFORE setting size
    gain_slider = std::make_unique<ParameterSlider>(state, PARAM::GAIN);
    addAndMakeVisible(*gain_slider);
    parameter_2_slider = std::make_unique<ParameterSlider>(state, PARAM::PARAM2);
    addAndMakeVisible(*parameter_2_slider);

    ...
}

// Position elements in resized
void AudioPluginAudioProcessorEditor::resized()
{
    // set the position of your components here
    auto slider_size = proportionOfWidth(0.1f);
    auto slider_1_x = proportionOfWidth(0.5f) - (slider_size / 2.0f);
    auto slider_2_x = slider_1_x + slider_size;
    auto slider_y = proportionOfHeight(0.5f) - (slider_size / 2.0f);
    gain_slider->setBounds(slider_1_x, slider_y, slider_size, slider_size);
    parameter_2_slider->setBounds(slider_2_x, slider_y, slider_size, slider_size);
}

// Trigger repaint calls on parameter changes in the TimerCallback
// Only repaint components relevant to that parameter's changes

void AudioPluginAudioProcessorEditor::timerCallback() {
    ...

    // handle parameter values in the UI (repaint relevant components)
    if (state->any_parameter_changed.exchange(false)) {
        if (state->get_parameter_modified(PARAM::GAIN)) {
            gain_slider->repaint();
        }
        if (state->get_parameter_modified(PARAM::PARAM2)) {
            parameter_2_slider->repaint();
        }
    }
    ...`
}

```

# Related Works and Resources

## Template Plugins

There are a number of existing open source plugin projects and templates. 

The Template Plugin is directly modified from the JUCE framework [CMake Audio Plugin Example](https://github.com/juce-framework/JUCE/tree/master/examples/CMake/AudioPlugin), included in the JUCE library. The CMake Audio Plugin Example provides a minimal framework for exporting audio plugins from JUCE with CMake. It comes with an `AudioPluginAudioProcessor` class for handling interactions with the audio thread and an `AudioPluginAudioProcessorEditor` class for handling interactions with the message thread. The CMake Audio Plugin Example is minimal and unopinionated; it does not provide any system for parameter management, leaving the developer to create their own real-time safe state management system. 

*Pamplejuce* is an open source JUCE Template that supports, along with CMake, a testing system with the Catch2 framework, C++20, and a GitHub Actions configuration for building cross platform, testing, and code-signing. *Pamplejuce* is developed by Sudara Williams and is licensed under the MIT license. *Pamplejuce* builds on the JUCE CMake Audio Plugin Example by adding features necessary for distributing professional plugins. Compared to The Template Plugin, *pamplejuce* has a less opinionated approach for state management, using essentially the same source code as the JUCE CMake Audio Plugin Example with a more sophisticated build system. 

Nicholas Berriochoa's *juce-template* is an open source audio plugin template which is most similar in scope to my Template Plugin. In particular, Nicholas Berriochoa's implementation includes logic for a state management system that wraps plugin parameters and internal plugin settings, as well as other useful features that go beyond The Template Plugin such as react-js user interfaces, Rust-based digital signal processing, testing, and key verification.

*Surge XT* is an award winning open source software synthesizer built in C++ with a JUCE-based user interface. *Surge XT* is released under the GNU GPLv3 license, and is a powerful reference for the digital signal processing and design patterns that go into a commercial-quality product. *Surge XT* supports a number of build platforms including MacOS, Windows, Linux, and Raspberry Pi. Furthermore, *Surge XT* can be built in the new CLAP plugin format. 

*valentine* is an open source compressor plugin developed by Jose Diaz Rohena. It is a great example of a simple plugin built using the JUCE framework. *valentine* is released under the GNU GPLv3 license. 

*Vital* is a popular "spectral warping wavetable synthesizer" by Matt Tytel released under GNU GPLv3. *Vital* is a massive project built on JUCE but with mostly custom code in C++. Furthermore, *Vital* uses hardware accelerated graphics for performant and detailed visualization in the UI. *Vital* is a great resource for understanding how a large plugin project can be organized. 

## Plugin Resources

Developing my own system for writing audio plugins has relied heavily on numerous plugin development tutorials and resources.

JUCE provides a series of beginner friendly [tutorials](https://juce.com/learn/tutorials/) for getting started with plugin development using the JUCE library. Unfortunately, some of these tutorials do not strictly adhere to the principals of real-time safe audio programming. Despite this, they are useful for developing an understanding of the JUCE framework. Furthermore, the [JUCE forum](https://forum.juce.com/) contains a plethora of valuable questions and conversations between developers. 

Joshua Hodge, better known as The Audio Programmer, has fostered a large community of audio developers around his YouTube and Discord channels. [The Audio Programmer YouTube channel](https://www.youtube.com/@TheAudioProgrammer) contains recordings of in-depth coding sessions and valuable interviews with developers. Furthermore, The Audio Programmer Discord community is one of the best places to search for plugin development questions, as many topics have been addressed in the Discord that are not posted on online forums. 

The Audio Developers Conference is an annual professional developers conference hosted by JUCE. Recordings of talks from the Audio Developers Conference are posted online at the [JUCE Youtube channel](https://www.youtube.com/@JUCElibrary/videos) – these talks are an essential resource for understanding some of the more subtle aspects of audio programming, such as real-time safe programming, distribution, testing, and new approaches. 

Timur Doumler's talk on [thread synchronization in real-time audio processing](https://www.youtube.com/watch?v=7fKxIZOyBCE) and Fabian Renn-Giles and Dave Rowland's talk [Real-Time 101](https://www.youtube.com/watch?v=Q0vrQFyAdWI) have been essential to my understanding of real-time programming and the development of this project. Timur Doumler has also released an open source library of low-latency real-time programming in C++, [Crill](https://github.com/crill-dev/crill), which implements some of the more sophisticated thread synchronization algorithms that I discuss in this paper. For another great reference on real-time safe audio programming, see Ross Bencina's blog post [Real-Time Audio Programming 101: Time Waits for Nothing](http://www.rossbencina.com/code/real-time-audio-programming-101-time-waits-for-nothing).

[Sudara Williams' blog](https://melatonin.dev/blog/) provides a number of detailed tutorials on optimizing, profiling, and testing audio plugins. This blog has informed my understanding of the JUCE repainting system and the best ways to profile plugin projects. 

The second edition of [The Computer Music Tutorial](https://www.amazon.com/Computer-Music-Tutorial-MIT-Press/dp/0262680823) by Curtis Roads is a comprehensive and essential reference for all aspects of computer music. Furthermore, Julius O. Smith has a number of [books and tutorials](https://ccrma.stanford.edu/~jos/pubs.html) that are freely available online about physical modeling, digital filters, and other digital signal processing topics. The comprehensive [DAFX](https://www.dafx.de/DAFX_Book_Page_2nd_edition/index.html) book is another useful reference for implementing digital audio effects.

Finally, Oli Larkin's [More Awesome Music DSP](https://github.com/olilarkin/awesome-musicdsp), Jared Drayton's [Audio Plugin Development Resources](https://github.com/jareddrayton/Audio-Plugin-Development-Resources), and Sudara William's [Awesome Juce](https://github.com/sudara/awesome-juce) are comprehensive lists of resources for audio programmers, which go well beyond the scope of this paper and include a number of additional references. 