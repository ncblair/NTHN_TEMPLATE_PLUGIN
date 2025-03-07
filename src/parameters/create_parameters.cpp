#include "customStringValueFunctions.h" // Defines CUSTOM_PARAMETER_VALUE_TO_STRING_FUNCTIONS and CUSTOM_PARAMETER_STRING_TO_VALUE_FUNCTIONS
#include <algorithm>
#include <array>
#include <fstream>
#include <functional>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>
#include <unordered_map>
#include <vector>

// Structure to hold parameter info.
struct Parameter {
  std::string param;
  std::string min;
  std::string max;
  std::string grain;
  std::string exp;
  std::string defaultVal;
  std::string automatable;
  std::string name;
  std::string suffix;
  std::string tooltip;
  std::vector<std::string> toStringArr;
};

// Simple trim helper.
static inline void trim(std::string &s) {
  s.erase(s.begin(),
          std::find_if(s.begin(), s.end(), [](unsigned char ch) { return !std::isspace(ch); }));
  s.erase(
      std::find_if(s.rbegin(), s.rend(), [](unsigned char ch) { return !std::isspace(ch); }).base(),
      s.end());
}

// If the literal has no decimal, append ".0f"; otherwise append "f".
std::string processFloatLiteral(const std::string &s) {
  return (s.find('.') == std::string::npos) ? s + ".0f" : s + "f";
}

int main() {
  std::ifstream csv("parameters.csv");
  if (!csv) {
    std::cerr << "Error: Could not open parameters.csv\n";
    return 1;
  }
  std::string header;
  std::getline(csv, header); // skip header

  std::vector<Parameter> params;
  std::string line;
  while (std::getline(csv, line)) {
    if (line.empty()) continue;
    std::vector<std::string> tokens;
    std::istringstream ss(line);
    std::string token;
    while (std::getline(ss, token, ',')) {
      trim(token);
      tokens.push_back(token);
    }
    if (tokens.size() < 11) tokens.resize(11, "");

    Parameter p;
    p.param = tokens[0];
    p.min = processFloatLiteral(tokens[1]);
    p.max = processFloatLiteral(tokens[2]);
    p.grain = processFloatLiteral(tokens[3]);
    p.exp = processFloatLiteral(tokens[4]);
    p.defaultVal = processFloatLiteral(tokens[5]);
    p.automatable = tokens[6];
    p.name = tokens[7];
    p.suffix = tokens[8];
    p.tooltip = tokens[9];

    std::istringstream arrStream(tokens[10]);
    std::string arrToken;
    while (arrStream >> arrToken) {
      if (arrToken.size() >= 2 && arrToken.front() == '"' && arrToken.back() == '"')
        arrToken = arrToken.substr(1, arrToken.size() - 2);
      p.toStringArr.push_back(arrToken);
    }
    params.push_back(p);
  }

  std::ofstream headerFile("ParameterDefines.h");
  if (!headerFile) {
    std::cerr << "Error: Could not open ParameterDefines.h for writing\n";
    return 1;
  }

  // Header and includes.
  headerFile << "#pragma once\n";
  headerFile << "#include <juce_core/juce_core.h>\n";
  headerFile << "#include <array>\n";
  headerFile << "#include <vector>\n";
  headerFile << "#include <functional>\n";
  headerFile << "#include <sstream>\n";
  headerFile << "#include <iomanip>\n";
  headerFile << "#include \"customStringValueFunctions.h\"\n\n";

  // Parameter enum.
  headerFile << "enum PARAM {\n";
  for (const auto &p : params)
    headerFile << "\t" << p.param << ",\n";
  headerFile << "\tTOTAL_NUMBER_PARAMETERS\n};\n\n";

  // Basic arrays.
  headerFile << "static const std::array<juce::Identifier, PARAM::TOTAL_NUMBER_PARAMETERS> "
                "PARAMETER_IDS{\n";
  for (const auto &p : params)
    headerFile << "\t\"" << p.param << "\",\n";
  headerFile << "};\n\n";

  headerFile
      << "static const std::array<juce::String, PARAM::TOTAL_NUMBER_PARAMETERS> PARAMETER_NAMES{\n";
  for (const auto &p : params)
    headerFile << "\t\"" << p.param << "\",\n";
  headerFile << "};\n\n";

  headerFile << "static const std::array<juce::NormalisableRange<float>, "
                "PARAM::TOTAL_NUMBER_PARAMETERS> PARAMETER_RANGES {\n";
  for (const auto &p : params)
    headerFile << "\tjuce::NormalisableRange<float>(" << p.min << ", " << p.max << ", " << p.grain
               << ", " << p.exp << "),\n";
  headerFile << "};\n\n";

  headerFile
      << "static const std::array<float, PARAM::TOTAL_NUMBER_PARAMETERS> PARAMETER_DEFAULTS {\n";
  for (const auto &p : params)
    headerFile << "\t" << p.defaultVal << ",\n";
  headerFile << "};\n\n";

  headerFile
      << "static const std::array<bool, PARAM::TOTAL_NUMBER_PARAMETERS> PARAMETER_AUTOMATABLE {\n";
  for (const auto &p : params)
    headerFile << "\t" << (p.automatable == "1" ? "true" : "false") << ",\n";
  headerFile << "};\n\n";

  headerFile << "static const std::array<juce::String, PARAM::TOTAL_NUMBER_PARAMETERS> "
                "PARAMETER_NICKNAMES{\n";
  for (const auto &p : params)
    headerFile << "\t\"" << p.name << "\",\n";
  headerFile << "};\n\n";

  headerFile << "static const std::array<juce::String, PARAM::TOTAL_NUMBER_PARAMETERS> "
                "PARAMETER_SUFFIXES {\n";
  for (const auto &p : params)
    headerFile << "\t\"" << p.suffix << "\",\n";
  headerFile << "};\n\n";

  headerFile << "static const std::array<juce::String, PARAM::TOTAL_NUMBER_PARAMETERS> "
                "PARAMETER_TOOLTIPS {\n";
  for (const auto &p : params)
    headerFile << "\t\"" << p.tooltip << "\",\n";
  headerFile << "};\n\n";

  headerFile << "static const std::array<std::vector<juce::String>, "
                "PARAM::TOTAL_NUMBER_PARAMETERS> PARAMETER_TO_STRING_ARRS {\n";
  for (const auto &p : params) {
    headerFile << "\tstd::vector<juce::String>{";
    for (const auto &s : p.toStringArr)
      headerFile << "\"" << s << "\", ";
    headerFile << "},\n";
  }
  headerFile << "};\n\n";

  // Precompute custom function pointers.
  headerFile << "// Precomputed custom function pointers for streamlined lambda functions.\n";
  for (const auto &p : params) {
    if (CUSTOM_PARAMETER_VALUE_TO_STRING_FUNCTIONS.find(p.param) !=
        CUSTOM_PARAMETER_VALUE_TO_STRING_FUNCTIONS.end()) {
      headerFile << "static const std::function<std::string(const float, const int)> "
                    "CUSTOM_VALUE_TO_STRING_"
                 << p.param << " = CUSTOM_PARAMETER_VALUE_TO_STRING_FUNCTIONS.at(\"" << p.param
                 << "\");\n";
    }
    if (CUSTOM_PARAMETER_STRING_TO_VALUE_FUNCTIONS.find(p.param) !=
        CUSTOM_PARAMETER_STRING_TO_VALUE_FUNCTIONS.end()) {
      headerFile << "static const std::function<float(const std::string&)> CUSTOM_STRING_TO_VALUE_"
                 << p.param << " = CUSTOM_PARAMETER_STRING_TO_VALUE_FUNCTIONS.at(\"" << p.param
                 << "\");\n";
    }
  }
  headerFile << "\n";

  // Array for converting value to string.
  headerFile << "static const std::array<std::function<juce::String(const float, const int)>, "
                "PARAM::TOTAL_NUMBER_PARAMETERS> PARAMETER_VALUE_TO_STRING_FUNCTIONS {\n";
  for (size_t i = 0; i < params.size(); i++) {
    const auto &p = params[i];
    headerFile << "\t[p_id = " << i
               << "](float value, int maximumStringLength) -> juce::String {\n";
    if (CUSTOM_PARAMETER_VALUE_TO_STRING_FUNCTIONS.find(p.param) !=
        CUSTOM_PARAMETER_VALUE_TO_STRING_FUNCTIONS.end()) {
      headerFile << "\t\treturn juce::String(CUSTOM_VALUE_TO_STRING_" << p.param
                 << "(value, maximumStringLength));\n";
    } else {
      headerFile << "\t\tauto to_string_size = PARAMETER_TO_STRING_ARRS[p_id].size();\n";
      headerFile << "\t\tjuce::String res;\n";
      headerFile
          << "\t\tif (to_string_size > 0 && static_cast<unsigned int>(value) < to_string_size) {\n";
      headerFile
          << "\t\t\tres = PARAMETER_TO_STRING_ARRS[p_id][static_cast<unsigned long>(value)];\n";
      headerFile << "\t\t} else {\n";
      headerFile << "\t\t\tstd::stringstream ss;\n";
      headerFile << "\t\t\tss << std::fixed << std::setprecision(2) << value;\n";
      headerFile << "\t\t\tres = juce::String(ss.str());\n";
      headerFile << "\t\t}\n";
      headerFile << "\t\tauto output = (res + \" \" + PARAMETER_SUFFIXES[p_id]);\n";
      headerFile << "\t\treturn maximumStringLength > 0 ? output.substring(0, "
                    "maximumStringLength) : output;\n";
    }
    headerFile << "\t},\n";
  }
  headerFile << "};\n\n";

  // Array for converting string to value.
  headerFile << "static const std::array<std::function<float(const juce::String&)>, "
                "PARAM::TOTAL_NUMBER_PARAMETERS> PARAMETER_STRING_TO_VALUE_FUNCTIONS {\n";
  for (size_t i = 0; i < params.size(); i++) {
    const auto &p = params[i];
    headerFile << "\t[p_id = " << i << "](juce::String text) -> float {\n";
    if (CUSTOM_PARAMETER_STRING_TO_VALUE_FUNCTIONS.find(p.param) !=
        CUSTOM_PARAMETER_STRING_TO_VALUE_FUNCTIONS.end()) {
      headerFile << "\t\treturn CUSTOM_STRING_TO_VALUE_" << p.param << "(text.toStdString());\n";
    } else {
      headerFile << "\t\ttext = text.upToFirstOccurrenceOf(\" \" + PARAMETER_SUFFIXES[p_id], "
                    "false, true);\n";
      headerFile << "\t\tauto to_string_size = PARAMETER_TO_STRING_ARRS[p_id].size();\n";
      headerFile << "\t\tif (to_string_size > 0) {\n";
      headerFile << "\t\t\tauto beg = PARAMETER_TO_STRING_ARRS[p_id].begin();\n";
      headerFile << "\t\t\tauto end = PARAMETER_TO_STRING_ARRS[p_id].end();\n";
      headerFile << "\t\t\tauto itFind = std::find(beg, end, text);\n";
      headerFile << "\t\t\tif (itFind == end) {\n";
      headerFile << "\t\t\t\tDBG(\"ERROR: Could not find text in PARAMETER_TO_STRING_ARRS\");\n";
      headerFile << "\t\t\t\treturn text.getFloatValue();\n";
      headerFile << "\t\t\t}\n";
      headerFile << "\t\t\treturn static_cast<float>(std::distance(beg, itFind));\n";
      headerFile << "\t\t}\n";
      headerFile << "\t\treturn text.getFloatValue();\n";
    }
    headerFile << "\t},\n";
  }
  headerFile << "};\n\n";

  return 0;
}