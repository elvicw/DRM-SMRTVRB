# Architecture

## Current state

Architecture decisions are provisional because the plugin does not exist yet. The target is a maintainable C++ VST3 with DSP independently testable from the host wrapper and UI.

## Framework and build

The adjacent DRM-RESHAPE project provides a local reference using JUCE 8.0.6 and CMake. DRM-SMRTVRB currently keeps its DSP core framework-independent. JUCE is dual licensed under commercial terms and AGPLv3; the appropriate product license and distribution model have not been selected. See the official [JUCE licensing page](https://juce.com/get-juce/) before integrating JUCE.

## Planned boundaries

- **DSP:** allocation-free processing components with independent tests.
- **Source profiles:** one explicit profile per Target x Engine pair, with source-specific wet-send filtering and reverb defaults.
- **Plugin/parameters:** host-facing wrapper, stable parameter IDs, automation, state serialization, and smoothing.
- **Host timing:** tempo and transport extraction with graceful fallback.
- **Preset system:** factory preset data and version-tolerant state recall.
- **UI:** hardware-inspired presentation attached to actual plugin parameters.
- **Metering/visualization:** audio-thread-safe data handoff to the UI.
- **Build/release:** Windows-only CI and reproducible VST3 packaging.

Keep audio processing real-time safe: no file I/O, blocking locks, or dynamic allocation in the audio callback.

## Implemented DSP boundary

`Source/ReverbEngines.*` currently provides wet-only Room, Spring, and Gated engines behind one prepared processor interface. Delay memory is allocated in `prepare`; `process` does not allocate. Plugin wrapping, host communication, state/presets, UI, and host-parameter automation are not implemented.

`Source/TargetProfiles.*` adds twelve initial source/engine profiles. `ProfiledReverb` conditions the wet send with target-specific one-pole high-pass/low-pass filters and send gain, then applies each profile's timing, decay, damping, density, width, and wet-level defaults. Scratch buffers are allocated in `prepare`; calls may be split internally when the host block exceeds the prepared size.

`Source/SpaceMacro.*` evaluates one independently tuned macro curve per target/engine pair, smooths its values over 20 ms by default, and mixes dry/wet output. It is not yet connected to host automation or a plugin wrapper.

`Source/TempoSync.*` supplies division math, Free/Host/Tap mode semantics, BPM fallback, tap interval tracking, and a timing smoother. Host transport/BPM extraction remains in the future plugin adapter.
