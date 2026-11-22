# Architecture

## Current state

The project now has an initial JUCE VST3/standalone wrapper around the independently testable C++ DSP. Windows is the intended release platform; current local plugin compilation on macOS is a development check only.

## Framework and build

The project uses JUCE 8.0.6 with CMake FetchContent. JUCE is dual licensed under commercial terms and AGPLv3; the appropriate product license and distribution model have not been selected. Resolve the license before distributing binaries. See the official [JUCE licensing page](https://juce.com/get-juce/).

## Planned boundaries

- **DSP:** allocation-free processing components with independent tests.
- **Source profiles:** one explicit profile per Target x Engine pair, with source-specific wet-send filtering and reverb defaults.
- **Plugin/parameters:** `Source/PluginProcessor.*` uses JUCE APVTS with stable initial parameter IDs, automation, XML state callbacks, factory program recall, and SPACE smoothing.
- **Host timing:** reads host BPM when available and routes Free/Host/Tap timing through the existing timing module; transport-state behavior is not yet used.
- **Preset system:** the eight factory states are exposed as host programs and recalled into APVTS parameters. User preset files are not implemented; the editor compares current values to the active factory state for its modified indicator.
- **UI:** `Source/PluginEditor.*` provides the hardware-inspired editor and parameter attachments for the real processor parameters.
- **Metering/visualization:** atomic peak/activity snapshots are polled by a bounded UI timer. Current meters are mono-combined peak bars without stereo separation, peak hold, or decay history.
- **Build/release:** Windows-only CI and reproducible VST3 packaging.

Keep audio processing real-time safe: no file I/O, blocking locks, or dynamic allocation in the audio callback.

## Implemented DSP boundary

`Source/ReverbEngines.*` provides wet-only Room, Spring, and Gated engines behind one prepared processor interface. Delay memory is allocated in `prepare`; `process` does not allocate. Host-specific behavior remains outside this DSP module in the JUCE processor/editor layer.

`Source/TargetProfiles.*` adds twelve initial source/engine profiles. `ProfiledReverb` conditions the wet send with target-specific one-pole high-pass/low-pass filters and send gain, then applies each profile's timing, decay, damping, density, width, and wet-level defaults. Scratch buffers are allocated in `prepare`; calls may be split internally when the host block exceeds the prepared size.

`Source/SpaceMacro.*` evaluates one independently tuned macro curve per target/engine pair, smooths its values over 20 ms by default, and mixes dry/wet output. The JUCE wrapper binds SPACE to a host-automatable APVTS parameter.

`Source/TempoSync.*` supplies division math, Free/Host/Tap mode semantics, BPM fallback, tap interval tracking, and a timing smoother. Host transport/BPM extraction remains in the future plugin adapter.

`Source/PluginState.*` defines the portable plugin state, five optional advanced overrides, eight factory presets, and a versioned text codec. The JUCE wrapper currently serializes APVTS XML for host state; it does not yet bridge the portable codec into that host state format.

## Current wrapper limitations

The wrapper accepts matching mono or stereo input/output layouts. It does not implement mono-to-stereo routing. The editor includes target/character, SPACE, timing, advanced overrides, factory preset selection, and basic input/output/activity indicators. Windows VST3 loading, REAPER behavior, host automation/state restore, resizing at minimum dimensions, and audio performance remain unverified.
