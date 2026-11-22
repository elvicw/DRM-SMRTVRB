# DRM-SMRTVRB

**DRM-SMRTVRB** is a purpose-built drum reverb plugin by **yeahwaah audio**. Its workflow centers on selecting a drum source, choosing a reverb character, and shaping the result with the SPACE macro.

The project has completed Phase 0 and is working through Phase 1. The first standalone DSP engine baseline exists, but no plugin functionality or distributable build is claimed yet.

## Planned V1

- Source profiles for Shells, Snare, Room, and Overheads.
- Distinct Room, Spring, and Gated reverb engines.
- Individually tuned SPACE macro behavior for all twelve source/engine combinations.
- Host tempo sync, free timing, tap tempo, advanced controls, and factory presets.
- Windows VST3 as the initial distribution target.

## Project documentation

See [ROADMAP.md](ROADMAP.md) for the detailed requirements and milestone order. Build, architecture, DSP, parameters, testing, status, issues, and handover notes are maintained in the corresponding Markdown files in this repository.

## Build status

The initial development machine is macOS. Windows VST3 build and REAPER validation have not yet been performed. See [BUILD_INSTRUCTIONS.md](BUILD_INSTRUCTIONS.md) for the current environment notes.

## Offline DSP renders

The `drmsmrtvrb_render` utility renders mono/stereo WAV through a wet-only engine. See [TESTING.md](TESTING.md) for formats, commands, and listening checks. Put private audio in the ignored `recordings/` directory; do not commit recordings.

Generate repeatable synthetic input fixtures with `python3 Tools/GenerateTestRecordings.py`, then render every fixture/engine pair with `python3 Tools/RenderFixtureMatrix.py`. These are procedural drum-like test signals, not recordings.
