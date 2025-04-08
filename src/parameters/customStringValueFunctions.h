// Custom string value function for our gain function

#pragma once
#include "../Util/Util.h"
#include <cassert>
#include <cstdio>
#include <functional>
#include <iostream>
#include <span>
#include <string>
#include <unordered_map>
inline std::unordered_map<std::string, std::function<float(const std::string &)>>
    CUSTOM_PARAMETER_STRING_TO_VALUE_FUNCTIONS = {
        {"GAIN", [](const std::string &str) { return nthn_utils::db2gain(std::stof(str)); }}};

// allow value to string functions to take in dependencies
inline std::unordered_map<
    std::string, std::function<std::string(const float, const int, const float *dependencies,
                                           const int num_dependencies)>>
    CUSTOM_PARAMETER_VALUE_TO_STRING_FUNCTIONS = {
        {"GAIN", [](const float value, const int maximumStringLength, const float *dependencies,
                    const int num_dependencies) {
           char buffer[32];
           if (dependencies[0] < 0.5f) {
             if (value <= 0.0f) return std::string("-inf dB");
             const float val = nthn_utils::gain2db(value);
             snprintf(buffer, sizeof(buffer), "%.2f dB", val);
           } else {
             snprintf(buffer, sizeof(buffer), "x%.2f", value);
           }
           return std::string(buffer);
         }}};
