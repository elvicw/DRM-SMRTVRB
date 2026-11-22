# Build Instructions

## Current status

The framework-independent DSP library, tests, and an initial JUCE 8.0.6 VST3/standalone wrapper are configured with CMake. Initial machine inspection found macOS, Apple clang 21.0.1, CMake 4.5.0, and Ninja. The plugin targets have compiled locally on macOS as a development check; Windows remains the only supported release target for V1 and has not been built or validated yet. The VST3 has not been loaded in a DAW.

## DSP core build

```sh
cmake -S . -B build -G Ninja -DCMAKE_BUILD_TYPE=Release
cmake --build build
ctest --test-dir build --output-on-failure
```

Windows CI currently builds/tests this DSP core only. It does not yet produce a plugin artifact.

## Offline WAV renderer

After building, run `./build/drmsmrtvrb_render --help` for options. The renderer accepts mono/stereo PCM16, PCM24, PCM32, and float32 RIFF/WAVE input. It writes wet-only float32 WAV and appends a four-second tail by default. For Windows multi-config builds, the executable is typically `build/Release/drmsmrtvrb_render.exe`.

## Framework and licensing

The wrapper fetches JUCE 8.0.6 through CMake FetchContent. JUCE offers its JUCE licence plans and AGPLv3 terms; verify which applies to this product before distributing a binary. The dependency is pinned to tag 8.0.6, but FetchContent still requires network access for a clean build. See JUCE's [licensing overview](https://juce.com/get-juce/) and [JUCE 8 EULA](https://juce.com/legal/juce-8-licence/).

To build the plugin and DSP tests on a machine with CMake, a C++17 compiler, and Git/network access:

```sh
cmake -S . -B build-plugin -G Ninja -DCMAKE_BUILD_TYPE=Release -DDRMSMRTVRB_BUILD_PLUGIN=ON
cmake --build build-plugin
ctest --test-dir build-plugin --output-on-failure
```

The VST3 and standalone targets are available for the current host platform. Their presence does not establish Windows compatibility or DAW validation.

The latest macOS build linked the VST3 and standalone targets, and CTest passed 6/6 after the meter, preset-status, and tail-smoothing edits. The edited Windows workflow has not run yet.

## Future Windows build

The intended release artifact is a Windows VST3, built and verified on a Windows environment. Use Visual Studio 2022 Build Tools and CMake on a Windows runner; document the exact toolchain and packaging command after the first Windows run. Do not report Windows support as validated based on the local macOS build.
