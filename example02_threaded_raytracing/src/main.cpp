constexpr unsigned int DEFAULT_THREAD_COUNT = 1;
constexpr unsigned int DEFAULT_WIDTH = 1600;
constexpr unsigned int DEFAULT_HEIGHT = 1600;
constexpr const char *DEFAULT_OUTPUT_FILE = "raytrace_out";

#include <iostream>
#include <string>

#include "argParse.hpp"
#include "rayTracer.hpp"

void printHelp() {
  std::cout
      << "Usage:\n"
         "-h | --help                Display this help message\n"
         "-t <integer>\n"
         "   | --threads <integer>   Set the number of threads to use "
         "(default 1)\n"
         "--width <integer>          Set the width of the output image\n"
         "--height <integer>         Set the height of the output image\n"
         "-o <filename>\n"
         "   | --output <filename>   Set the output filename for the image"
      << std::endl;
}

int main(int argc, char **argv) {
  unsigned int threadCount = DEFAULT_THREAD_COUNT;
  unsigned int outputWidth = DEFAULT_WIDTH;
  unsigned int outputHeight = DEFAULT_HEIGHT;
  std::string outputFile = DEFAULT_OUTPUT_FILE;

  {
    auto results = Ex02::ArgParse::parseArgs(argc, argv,
                                             {
                                                 // single args
                                                 "-h",
                                                 "--help",
                                             },
                                             {
                                                 // double args
                                                 "-t",
                                                 "--threads",
                                                 "--width",
                                                 "--height",
                                                 "-o",
                                                 "--output",
                                             });

    if (auto iter = results.find("-h"); iter != results.end()) {
      printHelp();
      return 0;
    }
    if (auto iter = results.find("--help"); iter != results.end()) {
      printHelp();
      return 0;
    }

    const auto setThreadCount = [&threadCount](auto iter) {
      try {
        threadCount = std::stoul(iter->second);
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
    if (auto iter = results.find("-t"); iter != results.end()) {
      if (int result = setThreadCount(iter); result != 0) {
        return result;
      }
    } else if (auto iter = results.find("--threads"); iter != results.end()) {
      if (int result = setThreadCount(iter); result != 0) {
        return result;
      }
    }
    if (threadCount <= 0) {
      std::cout << "ERROR: Thread count set to invalid value (" << threadCount
                << ')' << std::endl;
      return 3;
    }
    if (auto iter = results.find("--width"); iter != results.end()) {
      try {
        outputWidth = std::stoul(iter->second);
      } catch (const std::invalid_argument &e) {
        std::cout << "ERROR: Failed to parse width (invalid)" << std::endl;
        return 3;
      } catch (const std::out_of_range &e) {
        std::cout << "ERROR: Failed to parse width (out of range)" << std::endl;
        return 4;
      }
    }
    if (outputWidth == 0) {
      std::cout << "ERROR: width cannot be 0" << std::endl;
      return 7;
    }
    if (auto iter = results.find("--height"); iter != results.end()) {
      try {
        outputHeight = std::stoul(iter->second);
      } catch (const std::invalid_argument &e) {
        std::cout << "ERROR: Failed to parse height (invalid)" << std::endl;
        return 5;
      } catch (const std::out_of_range &e) {
        std::cout << "ERROR: Failed to parse height (out of range)"
                  << std::endl;
        return 6;
      }
    }
    if (outputHeight == 0) {
      std::cout << "ERROR: height cannot be 0" << std::endl;
      return 8;
    }
    if (auto iter = results.find("-o"); iter != results.end()) {
      outputFile = iter->second;
    } else if (auto iter = results.find("--output"); iter != results.end()) {
      outputFile = iter->second;
    }
    if (outputFile.empty()) {
      std::cout << "ERROR: Output filename is empty" << std::endl;
      return 12;
    }
  }

  //    auto pixels = Ex02::RT::renderGraySphere(
  //        outputWidth, outputHeight, threadCount);
  auto pixels =
      Ex02::RT::renderColorsWithSpheres(outputWidth, outputHeight, threadCount);

  pixels.writeToFile(outputFile);

  return 0;
}
