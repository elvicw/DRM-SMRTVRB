# Changelog

## Unreleased — foundation

- Created the private DRM-SMRTVRB project repository and initial documentation set.
- Recorded the project identity as DRM-SMRTVRB by yeahwaah audio.
- No plugin functionality or release artifact is included.

## Unreleased — DSP engine baseline

- Added independent Room, Spring, and Gated wet-only DSP engines.
- Added CMake build and impulse-response tests for engine output, separation, mono handling, and gated tail reduction.
- This baseline has not been integrated into a VST3 plugin or auditioned with real drum recordings.

## Unreleased — Phase 1 validation tools

- Added `drmsmrtvrb_render`, an offline wet-only WAV renderer for Room, Spring, and Gated.
- Added mono/stereo PCM16/24/32 and float32 WAV input, float32 output, and configurable rendered tail.
- Expanded automated checks across sample rates, block sizes, and parameter extremes.
- Added WAV read/write tests and ignored the local `recordings/` folder.
- Added deterministic synthetic kick/shell, snare, tom, room-mic, and overhead fixture generation plus a 15-case render-matrix script.

## Unreleased — target profile baseline

- Added distinct initial profiles for all four targets across Room, Spring, and Gated.
- Added source-specific wet-send filtering, send level, and engine-specific density/wet-level controls.
- Added `ProfiledReverb` and automated checks for all twelve combinations, including mono input and block chunking.

## Unreleased — SPACE macro baseline

- Added twelve target/engine SPACE curves that vary multiple reverb controls and per-profile wet caps.
- Added a default 20 ms smoother and dry/wet processor; SPACE 0% returns the exact dry input.
- Added offline renderer support for `--target` and `--space` and expanded tests for range, smoothing, dry endpoint, and wet output.

## Unreleased — tempo synchronization calculations

- Added six musical division calculations for Free, Host, and Tap timing modes.
- Added last-valid/default BPM fallback, tap-tempo interval averaging, gated timing clamp reporting, and a timing smoother.
- Added tests for tempo changes and missing source timing. Host transport extraction remains pending a plugin wrapper.

## Unreleased — advanced controls and preset state

- Added optional Pre-delay, Tone, Width, Mix, and Tail override composition on top of SPACE.
- Added the eight named factory presets from the roadmap.
- Added versioned state serialization with bounds checks and forward-compatible unknown fields.
