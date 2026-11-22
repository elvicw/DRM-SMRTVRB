# Project Status

- **Product:** DRM-SMRTVRB
- **Company:** yeahwaah audio
- **Version:** 0.0.0 (development)
- **Phase:** 6 — Hardware-inspired UI
- **Completed:** Standalone engines/profiles/SPACE; Free/Host/Tap timing calculations; five synthetic fixtures; optional advanced overrides; eight factory presets; portable versioned state codec.
- **Active tasks:** Phase 6 Windows plugin and host verification; JUCE 8.0.6 is selected for implementation, while commercial/AGPL distribution terms remain undecided.
- **Outstanding:** Run Windows VST3 CI; reproduce the REAPER program/parameter mismatch; test REAPER audio/automation/state/tempo; validate minimum-size/accessibility behavior; tune with real recordings.
- **Latest successful build:** macOS Apple clang 21.0.1, CMake 4.5.0, Ninja Release; JUCE VST3 and standalone linked after current editor/processor changes.
- **Latest successful test:** `ctest --test-dir build-plugin --output-on-failure` passed 6/6 after current editor/processor changes.
- **Windows CI:** Workflow now enables the JUCE VST3 target and archives the `.vst3`, but the edited workflow has not run. Previous DSP-only run 45008104694 passed configure, build, and tests on `windows-latest`.
- **Repository:** https://github.com/elvicw/DRM-SMRTVRB (private)

## Environment observations

- Current machine: macOS (Apple clang 21.0.1, CMake 4.5.0, Ninja available).
- Git 2.52.0 and GitHub CLI 2.104.0 are available; GitHub authentication is active for `elvicw`.
- No Windows cross-compiler was found in the initial PATH/Homebrew check.
- JUCE 8.0.6 is fetched through CMake FetchContent; clean builds require network access.
- JUCE distribution licensing remains unresolved and must be settled before binary distribution.
- Local REAPER 7.34 (macOS arm64) scanned and instantiated the VST3 and opened its editor. No audio was played. In that capture, the program label said “Tight Shells” while Character showed Spring, SPACE showed 37.37%, and the editor marked the preset modified; reproduce and resolve this state mismatch. Temporary install was removed and REAPER was closed.
- The standalone UI was opened and visually inspected at its default size; choice menus display labels and parameter values use concise units/precision. Minimum-size and accessibility checks are outstanding.
- Current source/docs/workflow changes are uncommitted. No CI run has been triggered for them.
- Offline WAV renderer is built as `drmsmrtvrb_render`; five local synthetic fixtures and fifteen profile renders are under ignored `recordings/`; no user recordings were available.
