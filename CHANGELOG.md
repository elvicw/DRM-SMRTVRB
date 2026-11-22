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
