# Build Instructions

## Current status

The framework-independent DSP library, tests, and JUCE 8.0.6 VST3/standalone wrapper are configured with CMake. The development machine is macOS with Apple clang 21.0.1, CMake 4.5.0, and Ninja. The plugin targets compile locally on macOS. Windows CI run [45185893939](https://github.com/elvicw/DRM-SMRTVRB/actions/runs/45185893939) configured and built the VST3, passed all six CTest targets, staged the bundle, and uploaded the artifact. Windows REAPER 7.42 host acceptance was completed locally on Windows x64: controlled SPACE dry/wet, tempo response at 80/160 BPM, automation envelope, project save/reopen, all eight factory presets, and UI floating editor/resize were validated.

## DSP core build

```sh
cmake -S . -B build -G Ninja -DCMAKE_BUILD_TYPE=Release
cmake --build build
ctest --test-dir build --output-on-failure
```

The Windows workflow builds and tests the project and uploads a Windows VST3 artifact. The latest recorded Windows workflow is [45185893939](https://github.com/elvicw/DRM-SMRTVRB/actions/runs/45185893939), including all six CTest targets and bundle staging. Windows REAPER 7.42 host validation was performed locally on Windows x64.

## Offline WAV renderer

After building, run `./build/drmsmrtvrb_render --help` for options. The renderer accepts mono/stereo PCM16, PCM24, PCM32, and float32 RIFF/WAVE input. It writes wet-only float32 WAV and appends a four-second tail by default. For Windows multi-config builds, the executable is typically `build/Release/drmsmrtvrb_render.exe`.

## Framework and licensing

The wrapper fetches JUCE 8.0.6 through CMake FetchContent. JUCE offers its JUCE licence plans and AGPLv3 terms; this project uses the AGPLv3 option (see `LICENSE`), so distributed binaries must be accompanied by the corresponding source. The dependency is pinned to tag 8.0.6, but FetchContent still requires network access for a clean build. See JUCE's [licensing overview](https://juce.com/get-juce/) and [JUCE 8 EULA](https://juce.com/legal/juce-8-licence/).

To build the plugin and DSP tests on a machine with CMake, a C++17 compiler, and Git/network access:

```sh
cmake -S . -B build-plugin -G Ninja -DCMAKE_BUILD_TYPE=Release -DDRMSMRTVRB_BUILD_PLUGIN=ON
cmake --build build-plugin
ctest --test-dir build-plugin --output-on-failure
```

The VST3 and standalone targets are available for the current host platform. Their presence does not establish Windows compatibility or DAW validation.

The latest recorded macOS build linked the VST3 and standalone targets. The latest recorded Windows CI run passed all six CTest targets and uploaded the VST3 bundle. See [AGENT_HANDOVER.md](AGENT_HANDOVER.md) for the exact validation scope and remaining host checks.

## Windows build

The intended release artifact is a Windows VST3. CI builds it on a Windows runner, but Windows REAPER validation remains outstanding. Do not report Windows host support as validated based on CI compilation or macOS host checks.

## Release packaging

`.github/workflows/release-package.yml` runs on a `v<major>.<minor>.<patch>` tag push or manual dispatch. The tag must match the `project(DRM_SMRTVRB VERSION ...)` value in `CMakeLists.txt`. It builds, runs CTest, and uploads `DRM-SMRTVRB-<version>-Windows-VST3.zip` plus a `.sha256` file as a 30-day workflow artifact. It does not create a GitHub Release, and no binary should be distributed until the JUCE licensing decision is made. Verified by manual dispatch on `main` (run 45210138012): artifact contained the zip and a matching SHA-256, with `x86_64-win/DRM-SMRTVRB.vst3` and `moduleinfo.json`. The tag/version mismatch check was not exercised.
