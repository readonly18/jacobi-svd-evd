cmake_minimum_required(VERSION 3.12)
set(SOURCES
    evd_classic.cpp
    evd_classic.hpp
    )

add_library(evd-classic-lib STATIC ${SOURCES})
target_include_directories(evd-classic-lib PUBLIC .)
target_link_libraries(evd-classic-lib utils-lib)
