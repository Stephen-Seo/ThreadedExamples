#!/usr/bin/env sh

set -ev

clang-tidy src/main.cpp -checks=*,-llvmlibc-*,-fuchsia-*,-readability-magic-numbers,-cppcoreguidelines-avoid-magic-numbers,-modernize-use-trailing-return-type,-llvm-header-guard,-cppcoreguidelines-pro-bounds-pointer-arithmetic,-altera-struct-pack-align,-misc-non-private-member-variables-in-classes,-modernize-use-nodiscard,-cppcoreguidelines-pro-type-union-access,-llvm-else-after-return,-readability-else-after-return -dump-config -- -std=c++17 > .clang-tidy 
