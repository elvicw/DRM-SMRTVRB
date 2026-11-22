cmake_minimum_required(VERSION 3.22)
project(DRM_SMRTVRB VERSION 0.1.0 LANGUAGES CXX)

option(DRMSMRTVRB_BUILD_TESTS "Build DSP validation tests" ON)

add_library(drmsmrtvrb_dsp STATIC
    Source/ReverbEngines.cpp
    Source/ReverbEngines.h
    Source/SpaceMacro.cpp
    Source/SpaceMacro.h
    Source/PluginState.cpp
    Source/PluginState.h
    Source/TargetProfiles.cpp
    Source/TargetProfiles.h
    Source/TempoSync.cpp
    Source/TempoSync.h)
target_include_directories(drmsmrtvrb_dsp PUBLIC Source)
target_compile_features(drmsmrtvrb_dsp PUBLIC cxx_std_17)
if(MSVC)
    target_compile_options(drmsmrtvrb_dsp PRIVATE /W4 /permissive-)
else()
    target_compile_options(drmsmrtvrb_dsp PRIVATE -Wall -Wextra -Wpedantic)
endif()

add_library(drmsmrtvrb_wav STATIC Tools/WavFile.cpp Tools/WavFile.h)
target_include_directories(drmsmrtvrb_wav PUBLIC Tools)
target_compile_features(drmsmrtvrb_wav PUBLIC cxx_std_17)

add_executable(drmsmrtvrb_render Tools/RenderWav.cpp)
target_link_libraries(drmsmrtvrb_render PRIVATE drmsmrtvrb_dsp drmsmrtvrb_wav)
target_compile_features(drmsmrtvrb_render PRIVATE cxx_std_17)

if(DRMSMRTVRB_BUILD_TESTS)
    enable_testing()
    add_executable(drmsmrtvrb_dsp_tests Tests/ReverbEnginesTests.cpp)
    target_link_libraries(drmsmrtvrb_dsp_tests PRIVATE drmsmrtvrb_dsp)
    add_test(NAME ReverbEngines COMMAND drmsmrtvrb_dsp_tests)
    add_executable(drmsmrtvrb_profile_tests Tests/TargetProfilesTests.cpp)
    target_link_libraries(drmsmrtvrb_profile_tests PRIVATE drmsmrtvrb_dsp)
    target_compile_features(drmsmrtvrb_profile_tests PRIVATE cxx_std_17)
    add_test(NAME TargetProfiles COMMAND drmsmrtvrb_profile_tests)
    add_executable(drmsmrtvrb_space_tests Tests/SpaceMacroTests.cpp)
    target_link_libraries(drmsmrtvrb_space_tests PRIVATE drmsmrtvrb_dsp)
    target_compile_features(drmsmrtvrb_space_tests PRIVATE cxx_std_17)
    add_test(NAME SpaceMacro COMMAND drmsmrtvrb_space_tests)
    add_executable(drmsmrtvrb_tempo_tests Tests/TempoSyncTests.cpp)
    target_link_libraries(drmsmrtvrb_tempo_tests PRIVATE drmsmrtvrb_dsp)
    target_compile_features(drmsmrtvrb_tempo_tests PRIVATE cxx_std_17)
    add_test(NAME TempoSync COMMAND drmsmrtvrb_tempo_tests)
    add_executable(drmsmrtvrb_state_tests Tests/PluginStateTests.cpp)
    target_link_libraries(drmsmrtvrb_state_tests PRIVATE drmsmrtvrb_dsp)
    target_compile_features(drmsmrtvrb_state_tests PRIVATE cxx_std_17)
    add_test(NAME PluginState COMMAND drmsmrtvrb_state_tests)
    add_executable(drmsmrtvrb_wav_tests Tests/WavFileTests.cpp)
    target_link_libraries(drmsmrtvrb_wav_tests PRIVATE drmsmrtvrb_wav)
    target_compile_features(drmsmrtvrb_wav_tests PRIVATE cxx_std_17)
    add_test(NAME WavFile COMMAND drmsmrtvrb_wav_tests)
endif()
