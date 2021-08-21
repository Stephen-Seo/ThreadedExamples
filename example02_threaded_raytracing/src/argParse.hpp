#ifndef EXAMPLE_02_ARG_PARSE_HPP
#define EXAMPLE_02_ARG_PARSE_HPP

#define EX02_ARG_PARSE_SINGLE_ARG_PLACEHOLDER "SINGLE_ARG_PLACEHOLDER"

#include <string>
#include <unordered_set>
#include <unordered_map>

namespace Ex02 {
namespace ArgParse {

typedef std::unordered_set<std::string> ArgsType;
typedef std::unordered_map<std::string, std::string> ParseResult;

ParseResult parseArgs(int argc, char **argv, const ArgsType &singleArgs, const ArgsType &doubleArgs);

} // namespace ArgParse
} // namespace Ex02

#endif
