#!/usr/bin/env bash

files=$(find . -name '*.hpp' -or -name '*.cpp' | grep -viE '\.\/build\/' | grep -viE '\.\/perf\/eigen\/' | grep -viE prettyprint.hpp)
clang-format -i ${files}
