#pragma once
#include <juce_core/juce_core.h>
enum PARAM {
	GAIN,
	TOTAL_NUMBER_PARAMETERS
};
static const std::array<juce::Identifier, PARAM::TOTAL_NUMBER_PARAMETERS> PARAMETER_IDS{
	"GAIN",
};
static const std::array<juce::String, PARAM::TOTAL_NUMBER_PARAMETERS> PARAMETER_NAMES{
	"GAIN",
};
static const std::array<juce::NormalisableRange<float>, PARAM::TOTAL_NUMBER_PARAMETERS> PARAMETER_RANGES {
	juce::NormalisableRange<float>(0.0f, 100.0f, 0.0f, 1.0f),
};
static const std::array<float, PARAM::TOTAL_NUMBER_PARAMETERS> PARAMETER_DEFAULTS {
	50.0f,
};
static const std::array<bool, PARAM::TOTAL_NUMBER_PARAMETERS> PARAMETER_AUTOMATABLE {
	true,
};
static const std::array<juce::String, PARAM::TOTAL_NUMBER_PARAMETERS> PARAMETER_NICKNAMES{
	"Gain",
};
static const std::array<juce::String, PARAM::TOTAL_NUMBER_PARAMETERS> PARAMETER_SUFFIXES {
	"%",
};
static const std::array<juce::String, PARAM::TOTAL_NUMBER_PARAMETERS> PARAMETER_TOOLTIPS {
	"Loudness Parameter",
};
static const std::array<std::vector<juce::String>, PARAM::TOTAL_NUMBER_PARAMETERS> PARAMETER_TO_STRING_ARRS {
	std::vector<juce::String>{},
};
