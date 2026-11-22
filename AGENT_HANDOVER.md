# Agent Handover

## Current state

- **Phase:** 6 — Hardware-inspired UI.
- **Last completed milestone:** Advanced override composition, eight factory presets, and versioned state codec pass local tests; Phase 2–4 DSP/timing modules are also implemented.
- **Active milestone:** Implement the Phase 6 editor after selecting the plugin framework and licensing model; framework-independent UI requirements are in `UI_SPEC.md`.
- **Implementation status:** DSP library, offline renderer, target profiles, SPACE, tempo calculator/tap tracker, advanced controls, preset bank/state codec, and six CTest targets exist. No VST3 plugin, host transport adapter, or UI.

## Completed work

- Repository: https://github.com/elvicw/DRM-SMRTVRB (private).
- Product name: DRM-SMRTVRB. Company: yeahwaah audio.
- The incoming roadmap was moved to ROADMAP.md with product naming updated.
- Adjacent `../drum-shaper` is DRM-RESHAPE and uses CMake with JUCE 8.0.6 FetchContent. This is a reference only.
- `Source/ReverbEngines.*` implements early-reflection/tank Room, resonant/dispersive Spring, and separately gated feedback engines. Processing does not allocate; delay memory is allocated in `prepare`.
- `Source/TargetProfiles.*` defines twelve distinct profiles with source-specific wet-send filtering and engine controls.
- `Source/SpaceMacro.*` maps 0–100% through twelve independent curves, equal-power dry/wet gains, and a default 20 ms smoother. `SpaceMacroProcessor` returns exactly dry at 0% and processes parameters in 32-sample quanta.
- `Source/TempoSync.*` calculates six musical divisions for Free/Host/Tap modes, retains valid fallback BPM values, handles tap intervals, and smooths timing changes. Actual host BPM extraction is not implemented.
- `Source/PluginState.*` composes advanced overrides after SPACE and contains the eight roadmap presets plus a versioned text state codec. The codec is not connected to host state callbacks.
- `UI_SPEC.md` captures the framework-independent Phase 6 layout, interaction, parameter/state contract, and implementation acceptance checks.
- `Tools/RenderWav.cpp` and `Tools/WavFile.*` provide an offline mono/stereo WAV renderer. Supported input is PCM16/24/32 and float32; manual engine mode writes wet-only float32, while profile/SPACE mode applies its dry/wet mix.
- `Tools/GenerateTestRecordings.py` creates five procedural drum-like fixtures (kick/shells, snare, toms, room-mic, overheads); `Tools/RenderFixtureMatrix.py` renders all 15 profile/engine pairs at SPACE 65. Generated WAVs are local under ignored `recordings/`.
- `Tests/ReverbEnginesTests.cpp`, `Tests/TargetProfilesTests.cpp`, and `Tests/SpaceMacroTests.cpp` cover engine safety, all twelve profiles, SPACE range/smoothing/exact dry endpoint, mono, and block chunking.
- `Tests/TempoSyncTests.cpp` and `Tests/PluginStateTests.cpp` cover timing divisions/fallback/tap tracking, advanced override interactions, factory presets, and state serialization.
- `Tests/WavFileTests.cpp` checks float32 round trip and PCM16/24/32 decoding.
- Synthetic outputs were finite and below 0 dBFS in the generated matrix. They are not real recordings; no actual drum recordings have been supplied, so real-material listening is still outstanding.

## Build and test status

- Machine: macOS; Apple clang 21.0.1, CMake 4.5.0, Ninja.
- Build: `cmake -S . -B build -G Ninja -DCMAKE_BUILD_TYPE=Release && cmake --build build`
- Test: `ctest --test-dir build --output-on-failure`
- Render help: `./build/drmsmrtvrb_render --help`
- Latest local result: build succeeded; CTest passed 6/6 on macOS Apple clang 21.0.1. All five synthetic inputs rendered through all twelve source/engine profiles at SPACE 65; all output peaks remained below 0 dBFS.
- Windows CI run 45008104694 passed configure, build, and all six CTest targets including Phase 5. Windows run 45008427219 passed configure, build, and all six CTest targets after commit `02c0273`.
- No actual drum recordings were available; no real-material listening, REAPER validation, or plugin build has been performed.
- Windows VST3 and REAPER validation remain required.

## GitHub status

- Owner/account: `elvicw`.
- Repository URL: https://github.com/elvicw/DRM-SMRTVRB.
- Repository is private; current branch is `main`, tracking `origin/main`.
- Latest pushed commit at handover preparation: `02c027382ceac955e0cb0dc5a1831e3bcc3b6553` (`fix: guard DSP controls against non-finite values`).
- The UI specification and synchronized roadmap/status updates are included in the current documentation change.

## Next steps

1. Review `UI_SPEC.md`; choose the plugin framework and licensing/distribution path before implementing the wrapper/editor.
2. Assign stable host parameter IDs and connect host BPM/transport, state callbacks, automation, and preset recall in the selected framework.
3. Implement the editor and safe meter/envelope data handoff against the UI specification.
4. Continue through Phase 7; generated inputs are not real recordings and do not establish musical quality.
5. Validate in Windows REAPER, then package and document the plugin without claiming actual drum listening until real recordings are auditioned.

## Next agent instruction

Start by checking the `main` branch, repository status, and this handover. Phase 5 passes macOS tests and Windows CI; the non-finite input guards pass local CTest and Windows workflow 45008427219. Read `UI_SPEC.md`. The only user-dependent decision is framework and licensing/distribution choice; do not start a framework-dependent VST3 wrapper until that is selected. Do not claim real-recording audition or plugin validation.
