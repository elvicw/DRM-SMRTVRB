# Project Status

- **Product:** DRM-SMRTVRB
- **Company:** yeahwaah audio
- **Version:** 0.0.0 (development)
- **Phase:** 6 — Hardware-inspired UI
- **Completed:** Standalone engines/profiles/SPACE; Free/Host/Tap timing calculations; five synthetic fixtures; optional advanced overrides; eight factory presets; portable versioned state codec.
- **Active tasks:** Phase 6 UI specification is ready; choose framework and license model before implementing the wrapper/editor.
- **Outstanding:** Host BPM extraction, VST3 state callbacks, host parameter binding, preset browser, UI/meters, Windows/REAPER plugin validation, packaging, release, and real-recording tuning.
- **Latest successful build:** macOS Apple clang 21.0.1, CMake Release; local DSP build succeeded.
- **Latest successful test:** CTest passed 6/6 (`ReverbEngines`, `TargetProfiles`, `SpaceMacro`, `TempoSync`, `PluginState`, `WavFile`) on macOS.
- **Windows CI:** Run 45008104694 passed configure, build, and all six CTest targets on `windows-latest`.
- **Repository:** https://github.com/elvicw/DRM-SMRTVRB (private)

## Environment observations

- Current machine: macOS (Apple clang 21.0.1, CMake 4.5.0, Ninja available).
- Git 2.52.0 and GitHub CLI 2.104.0 are available; GitHub authentication is active for `elvicw`.
- No Windows cross-compiler or JUCE package was found in the initial PATH/Homebrew check.
- The adjacent `../drum-shaper` directory contains the DRM-RESHAPE source project.
- Its CMake config fetches JUCE 8.0.6. This architecture is a reference only; framework/license choice remains open.
- Offline WAV renderer is built as `drmsmrtvrb_render`; five local synthetic fixtures and fifteen profile renders are under ignored `recordings/`; no user recordings were available.
