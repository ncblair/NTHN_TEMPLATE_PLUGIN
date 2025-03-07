#pragma once
#include <juce_core/juce_core.h>
#include <array>
#include <vector>
#include <functional>
#include <sstream>
#include <iomanip>
#include "customStringValueFunctions.h"

enum PARAM {
	GAIN,
	MODE,
	MIX,
	TOTAL_NUMBER_PARAMETERS
};

static const std::array<juce::Identifier, PARAM::TOTAL_NUMBER_PARAMETERS> PARAMETER_IDS{
	"GAIN",
	"MODE",
	"MIX",
};

static const std::array<juce::String, PARAM::TOTAL_NUMBER_PARAMETERS> PARAMETER_NAMES{
	"GAIN",
	"MODE",
	"MIX",
};

static const std::array<juce::NormalisableRange<float>, PARAM::TOTAL_NUMBER_PARAMETERS> PARAMETER_RANGES {
	juce::NormalisableRange<float>(0.0f, 1.0f, 0.0f, 1.0f),
	juce::NormalisableRange<float>(0.0f, 1.0f, 1.0f, 1.0f),
	juce::NormalisableRange<float>(0.0f, 100.0f, 0.0f, 1.0f),
};

static const std::array<float, PARAM::TOTAL_NUMBER_PARAMETERS> PARAMETER_DEFAULTS {
	0.5f,
	0.0f,
	50.0f,
};

static const std::array<bool, PARAM::TOTAL_NUMBER_PARAMETERS> PARAMETER_AUTOMATABLE {
	true,
	true,
	true,
};

static const std::array<juce::String, PARAM::TOTAL_NUMBER_PARAMETERS> PARAMETER_NICKNAMES{
	"Gain",
	"Mode",
	"Mix",
};

static const std::array<juce::String, PARAM::TOTAL_NUMBER_PARAMETERS> PARAMETER_SUFFIXES {
	"",
	"",
	"%",
};

static const std::array<juce::String, PARAM::TOTAL_NUMBER_PARAMETERS> PARAMETER_TOOLTIPS {
	"Loudness Parameter",
	"Mode",
	"Mix",
};

static const std::array<std::vector<juce::String>, PARAM::TOTAL_NUMBER_PARAMETERS> PARAMETER_TO_STRING_ARRS {
	std::vector<juce::String>{},
	std::vector<juce::String>{"Decibels", "Amplitude", },
	std::vector<juce::String>{},
};

static const std::array<std::vector<juce::Identifier>, PARAM::TOTAL_NUMBER_PARAMETERS> PARAMETER_DEPENDENCY_IDS {
	std::vector<juce::Identifier>{"MODE", },
	std::vector<juce::Identifier>{},
	std::vector<juce::Identifier>{},
};

// Precomputed custom function pointers for streamlined lambda functions.
static const std::function<std::string(const float, const int, const float*, const int)> CUSTOM_VALUE_TO_STRING_GAIN = CUSTOM_PARAMETER_VALUE_TO_STRING_FUNCTIONS.at("GAIN");
static const std::function<float(const std::string&)> CUSTOM_STRING_TO_VALUE_GAIN = CUSTOM_PARAMETER_STRING_TO_VALUE_FUNCTIONS.at("GAIN");

static const std::array<std::function<juce::String(const float, const int, const float*, const int)>, PARAM::TOTAL_NUMBER_PARAMETERS> PARAMETER_VALUE_TO_STRING_FUNCTIONS {
	[p_id = 0](float value, int maximumStringLength, const float *dependencies, int num_dependencies) -> juce::String {
		return juce::String(CUSTOM_VALUE_TO_STRING_GAIN(value, maximumStringLength, dependencies, num_dependencies));
	},
	[p_id = 1](float value, int maximumStringLength, const float *dependencies, int num_dependencies) -> juce::String {
		auto to_string_size = PARAMETER_TO_STRING_ARRS[p_id].size();
		juce::String res;
		if (to_string_size > 0 && static_cast<unsigned int>(value) < to_string_size) {
			res = PARAMETER_TO_STRING_ARRS[p_id][static_cast<unsigned long>(value)];
		} else {
			std::stringstream ss;
			ss << std::fixed << std::setprecision(2) << value;
			res = juce::String(ss.str());
		}
		auto output = (res + " " + PARAMETER_SUFFIXES[p_id]);
		return maximumStringLength > 0 ? output.substring(0, maximumStringLength) : output;
	},
	[p_id = 2](float value, int maximumStringLength, const float *dependencies, int num_dependencies) -> juce::String {
		auto to_string_size = PARAMETER_TO_STRING_ARRS[p_id].size();
		juce::String res;
		if (to_string_size > 0 && static_cast<unsigned int>(value) < to_string_size) {
			res = PARAMETER_TO_STRING_ARRS[p_id][static_cast<unsigned long>(value)];
		} else {
			std::stringstream ss;
			ss << std::fixed << std::setprecision(2) << value;
			res = juce::String(ss.str());
		}
		auto output = (res + " " + PARAMETER_SUFFIXES[p_id]);
		return maximumStringLength > 0 ? output.substring(0, maximumStringLength) : output;
	},
};

static const std::array<std::function<float(const juce::String&)>, PARAM::TOTAL_NUMBER_PARAMETERS> PARAMETER_STRING_TO_VALUE_FUNCTIONS {
	[p_id = 0](juce::String text) -> float {
		return CUSTOM_STRING_TO_VALUE_GAIN(text.toStdString());
	},
	[p_id = 1](juce::String text) -> float {
		text = text.upToFirstOccurrenceOf(" " + PARAMETER_SUFFIXES[p_id], false, true);
		auto to_string_size = PARAMETER_TO_STRING_ARRS[p_id].size();
		if (to_string_size > 0) {
			auto beg = PARAMETER_TO_STRING_ARRS[p_id].begin();
			auto end = PARAMETER_TO_STRING_ARRS[p_id].end();
			auto itFind = std::find(beg, end, text);
			if (itFind == end) {
				DBG("ERROR: Could not find text in PARAMETER_TO_STRING_ARRS");
				return text.getFloatValue();
			}
			return static_cast<float>(std::distance(beg, itFind));
		}
		return text.getFloatValue();
	},
	[p_id = 2](juce::String text) -> float {
		text = text.upToFirstOccurrenceOf(" " + PARAMETER_SUFFIXES[p_id], false, true);
		auto to_string_size = PARAMETER_TO_STRING_ARRS[p_id].size();
		if (to_string_size > 0) {
			auto beg = PARAMETER_TO_STRING_ARRS[p_id].begin();
			auto end = PARAMETER_TO_STRING_ARRS[p_id].end();
			auto itFind = std::find(beg, end, text);
			if (itFind == end) {
				DBG("ERROR: Could not find text in PARAMETER_TO_STRING_ARRS");
				return text.getFloatValue();
			}
			return static_cast<float>(std::distance(beg, itFind));
		}
		return text.getFloatValue();
	},
};

