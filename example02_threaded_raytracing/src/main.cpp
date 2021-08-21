#include <iostream>
#include <string>

#include "argParse.hpp"

void printHelp() {
    std::cout << "Usage:\n"
        "-h | --help                Display this help message\n"
        "-t <integer>\n"
        "   | --threads <integer>   Set the number of threads to use (default 1)"
        << std::endl;
}

int main(int argc, char **argv) {
    int threadCount = 1;

    {
        auto results = Ex02::ArgParse::parseArgs(
            argc,
            argv,
            { // single args
                "-h",
                "--help",
            },
            { // double args
                "-t",
                "--threads",
            });

        if(auto iter = results.find("-h"); iter != results.end()) {
            printHelp();
            return 0;
        } else if(auto iter = results.find("--help"); iter != results.end()) {
            printHelp();
            return 0;
        }

        const auto setThreadCount = [&threadCount] (auto iter) {
            try {
                threadCount = std::stoi(iter->second);
            } catch (const std::invalid_argument &e) {
                std::cout << "ERROR: Failed to parse thread count (invalid)"
                    << std::endl;
                return 1;
            } catch (const std::out_of_range &e) {
                std::cout << "ERROR: Failed to parse thread count (out of range)"
                    << std::endl;
                return 2;
            }
            return 0;
        };
        if(auto iter = results.find("-t"); iter != results.end()) {
            if(int result = setThreadCount(iter); result != 0) {
                return result;
            }
        } else if(auto iter = results.find("--threads"); iter != results.end()) {
            if(int result = setThreadCount(iter); result != 0) {
                return result;
            }
        }

        if(threadCount <= 0) {
            std::cout << "ERROR: Thread count set to invalid value ("
                << threadCount
                << ')'
                << std::endl;
            return 3;
        }
    }

    return 0;
}
