# Architecture

## Current state

Architecture decisions are provisional because the plugin does not exist yet. The target is a maintainable C++ VST3 with DSP independently testable from the host wrapper and UI.

## Framework and build

The adjacent DRM-RESHAPE project provides a local reference using JUCE 8.0.6 and CMake. Evaluate that approach for DRM-SMRTVRB, including JUCE licensing and reproducible Windows dependency acquisition, before implementation.

## Planned boundaries

- **DSP:** allocation-free processing components with independent tests.
- **Plugin/parameters:** host-facing wrapper, stable parameter IDs, automation, state serialization, and smoothing.
- **Host timing:** tempo and transport extraction with graceful fallback.
- **Preset system:** factory preset data and version-tolerant state recall.
- **UI:** hardware-inspired presentation attached to actual plugin parameters.
- **Metering/visualization:** audio-thread-safe data handoff to the UI.
- **Build/release:** Windows-only CI and reproducible VST3 packaging.

Keep audio processing real-time safe: no file I/O, blocking locks, or dynamic allocation in the audio callback.
