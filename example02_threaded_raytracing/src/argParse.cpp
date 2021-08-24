#include "argParse.hpp"

#include <cstring>

Ex02::ArgParse::ParseResult
Ex02::ArgParse::parseArgs(int argc, char **argv,
                          const Ex02::ArgParse::ArgsType &singleArgs,
                          const Ex02::ArgParse::ArgsType &doubleArgs) {
  ParseResult result;
  bool found = false;
  --argc;
  ++argv;
  while (argc > 0) {
    found = false;
    for (const std::string &singleArg : singleArgs) {
      if (std::strcmp(argv[0], singleArg.c_str()) == 0) {
        result.insert({singleArg, EX02_ARG_PARSE_SINGLE_ARG_PLACEHOLDER});
        found = true;
        break;
      }
    }
    if (!found && argc > 1) {
      for (const std::string &doubleArg : doubleArgs) {
        if (std::strcmp(argv[0], doubleArg.c_str()) == 0) {
          result.insert({doubleArg, argv[1]});
          --argc;
          ++argv;
          break;
        }
      }
    }
    --argc;
    ++argv;
  }

  return result;
}
