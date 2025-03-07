// Custom string value function for our gain function

#pragma once
#include "../Util/Util.h"
#include <cstdio>
#include <functional>
#include <string>
#include <unordered_map>

inline std::unordered_map<std::string, std::function<float(const std::string &)>>
    CUSTOM_PARAMETER_STRING_TO_VALUE_FUNCTIONS = {
        {"GAIN", [](const std::string &str) { return nthn_utils::db2gain(std::stof(str)); }}};

inline std::unordered_map<std::string, std::function<std::string(const float, const int)>>
    CUSTOM_PARAMETER_VALUE_TO_STRING_FUNCTIONS = {
        {"GAIN", [](const float value, const int maximumStringLength) {
           char buffer[32];
           if (value <= 0.0f) return std::string("-inf dB");
           snprintf(buffer, sizeof(buffer), "%.2f dB", nthn_utils::gain2db(value));
           return std::string(buffer);
         }}};
