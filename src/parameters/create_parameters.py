# Nathan Blair 2022

import os 

def main():
    dir_path = os.path.dirname(os.path.realpath(__file__))

    with open(os.path.join(dir_path, "parameters.csv")) as f:
        lines = [[x.strip() for x in line.split(",")] for line in f.readlines()[1:]]
    parameters = [l[0] for l in lines]
    ranges = [l[1:5] for l in lines]

    for i, r in enumerate(ranges):
        for j, f in enumerate(r):
            if not ("." in f):
                ranges[i][j] += ".0f"
            else:
                ranges[i][j] += "f"
                
    defaults = [l[5] for l in lines]

    for i, d in enumerate(defaults):
        if not ("." in d):
            defaults[i] += ".0f"
        else:
            defaults[i] += "f"

    automatable = [l[6] for l in lines]

    names = [l[7] for l in lines]
    suffixes = [l[8] for l in lines]
    tooltips = [l[9] for l in lines]
    to_string_arrs = [l[10] for l in lines]
    to_string_arrs = [x.split() for x in to_string_arrs]

    formatted = ["#pragma once", '#include <juce_core/juce_core.h>']

    param_enum = ["enum PARAM {"]
    param_enum += ["\t" + p + "," for p in parameters]
    param_enum += ["\t" + "TOTAL_NUMBER_PARAMETERS"]
    param_enum += ["};"]

    param_ids = ["static const std::array<juce::Identifier, PARAM::TOTAL_NUMBER_PARAMETERS> PARAMETER_IDS{"]
    param_ids += ['\t"' + p + '",' for p in parameters]
    param_ids += ["};"]

    param_names = ["static const std::array<juce::String, PARAM::TOTAL_NUMBER_PARAMETERS> PARAMETER_NAMES{"]
    param_names += ['\t"' + p + '",' for p in parameters]
    param_names += ["};"]

    param_ranges = ["static const std::array<juce::NormalisableRange<float>, PARAM::TOTAL_NUMBER_PARAMETERS> PARAMETER_RANGES {"]
    param_ranges += [f"\tjuce::NormalisableRange<float>({r[0]}, {r[1]}, {r[2]}, {r[3]})," for r in ranges]
    param_ranges += ["};"]

    param_defaults = ["static const std::array<float, PARAM::TOTAL_NUMBER_PARAMETERS> PARAMETER_DEFAULTS {"]
    param_defaults += [f"\t{d}," for d in defaults]
    param_defaults += ["};"]

    param_automatable = ["static const std::array<bool, PARAM::TOTAL_NUMBER_PARAMETERS> PARAMETER_AUTOMATABLE {"]
    param_automatable += [f'\t{"true" if a == "1" else "false"},' for a in automatable]
    param_automatable += ["};"]

    param_nicknames = ["static const std::array<juce::String, PARAM::TOTAL_NUMBER_PARAMETERS> PARAMETER_NICKNAMES{"]
    param_nicknames += [f'\t"{n}",' for n in names]
    param_nicknames += ["};"]

    param_suffixes = ["static const std::array<juce::String, PARAM::TOTAL_NUMBER_PARAMETERS> PARAMETER_SUFFIXES {"]
    param_suffixes += [f'\t"{s}",' for s in suffixes]
    param_suffixes += ["};"]

    param_tooltips = ["static const std::array<juce::String, PARAM::TOTAL_NUMBER_PARAMETERS> PARAMETER_TOOLTIPS {"]
    param_tooltips += [f'\t"{t}",' for t in tooltips]
    param_tooltips += ["};"]

    param_to_str = ["static const std::array<std::vector<juce::String>, PARAM::TOTAL_NUMBER_PARAMETERS> PARAMETER_TO_STRING_ARRS {"]
    for strings in to_string_arrs:
        param_to_str += ['\tstd::vector<juce::String>{']
        param_to_str += [f'\t\t{s},' for s in strings]
        param_to_str += ['\t},']
    param_to_str += ["};"]

    formatted += param_enum + param_ids + param_names + param_ranges + param_defaults + param_automatable + param_nicknames + param_suffixes + param_tooltips + param_to_str

    with open(os.path.join(dir_path, "ParameterDefines.h"), "w") as f:
        f.write('\n'.join(formatted))

if __name__ == "__main__":
    main()