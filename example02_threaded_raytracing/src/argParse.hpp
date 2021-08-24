#ifndef EXAMPLE_02_ARG_PARSE_HPP
#define EXAMPLE_02_ARG_PARSE_HPP

constexpr const char *EX02_ARG_PARSE_SINGLE_ARG_PLACEHOLDER =
    "SINGLE_ARG_PLACEHOLDER";

#include <string>
#include <unordered_map>
#include <unordered_set>

namespace Ex02::ArgParse {

using ArgsType = std::unordered_set<std::string>;
using ParseResult = std::unordered_map<std::string, std::string>;

ParseResult parseArgs(int argc, char **argv, const ArgsType &singleArgs,
                      const ArgsType &doubleArgs);

} // namespace Ex02::ArgParse

#endif
