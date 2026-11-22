cmake_minimum_required(VERSION 3.22)
project(DRM_SMRTVRB VERSION 0.1.0 LANGUAGES CXX)

option(DRMSMRTVRB_BUILD_TESTS "Build DSP validation tests" ON)

add_library(drmsmrtvrb_dsp STATIC
    Source/ReverbEngines.cpp
    Source/ReverbEngines.h)
target_include_directories(drmsmrtvrb_dsp PUBLIC Source)
target_compile_features(drmsmrtvrb_dsp PUBLIC cxx_std_17)
if(MSVC)
    target_compile_options(drmsmrtvrb_dsp PRIVATE /W4 /permissive-)
else()
    target_compile_options(drmsmrtvrb_dsp PRIVATE -Wall -Wextra -Wpedantic)
endif()

if(DRMSMRTVRB_BUILD_TESTS)
    enable_testing()
    add_executable(drmsmrtvrb_dsp_tests Tests/ReverbEnginesTests.cpp)
    target_link_libraries(drmsmrtvrb_dsp_tests PRIVATE drmsmrtvrb_dsp)
    add_test(NAME ReverbEngines COMMAND drmsmrtvrb_dsp_tests)
endif()
