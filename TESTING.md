# Testing

`Tests/ReverbEnginesTests.cpp` exercises Room, Spring, and Gated using stereo impulse input, checks finite/bounded output and nonzero tails, checks that the engine responses differ, checks mono processing, and checks gated tail reduction after its selected gate interval.

Latest local result: CMake Release build succeeded on macOS with Apple clang 21.0.1; CTest passed 5/5 (`ReverbEngines`, `TargetProfiles`, `SpaceMacro`, `TempoSync`, `WavFile`).

The first Windows CI run compiled successfully but the test executable segfaulted because the harness stored multiple full render buffers on the Windows thread stack. Moving these buffers to heap-backed vectors fixed the issue. Windows run 45004120337 passed configure, build, and both CTest targets on `windows-latest`.

Remaining coverage includes tempo sync and missing tempo, transport and host automation, preset/state restoration, long runs, CPU/memory behavior, actual drum recordings, and final REAPER validation on Windows.

## Phase 1 stress checks

The automated stress test renders repeated drum-like hits and deterministic noise across 44.1, 48, and 96 kHz; block sizes 32, 64, 128, 256, and 1024; all three engines; and alternating parameter extremes: decay 0.08/8 s, pre-delay 0/200 ms, damping 0/1, width 0/2, gate 0.03/4 s, density 0/1, and wet level 0/2. It checks that samples remain finite and peaks bounded.

`TargetProfiles` checks all twelve distinct source/engine profiles for valid ranges, finite/bounded wet output, and mono/stereo processing. `SpaceMacro` checks all twelve curve endpoints and bounds, 20 ms smoothing, exact dry output at zero, and wet output after automation. The render matrix uses each source profile and SPACE 65 for the synthetic source fixtures.

`TempoSync` checks all six subdivision calculations, BPM changes and missing-host fallback, Free mode, Tap mode fallback, tap averaging/outlier behavior, natural decay vs gated termination limits, and a 25 ms timing ramp. It does not test host transport extraction because no plugin framework/wrapper exists yet.

## Listening with drum recordings

The `drmsmrtvrb_render` utility accepts mono/stereo PCM16, PCM24, PCM32, or float32 WAV. It produces wet-only float32 WAV and appends four seconds by default. Example from the repository root:

```sh
./build/drmsmrtvrb_render --engine room \
  --input recordings/snare.wav --output build/snare-room-wet.wav \
  --decay 1.1 --predelay 8 --damping 0.45 --width 1 --tail 4
```

Render all three engines on kick/shells, snare, tom, room-mic, and overhead recordings. Listen to dry and wet at matched playback level; check kick low-end buildup, snare transient definition, cymbal wash, stereo stability, and gated tail timing. Manual engine renders are wet-only; `--target ... --space N` exercises the profile and SPACE crossfade. Private source files belong in the git-ignored `recordings/` folder.

No actual drum recordings were available in the workspace. Listening validation with real material remains outstanding; record the source, settings, observations, and any problematic samples here before marking Phase 1 complete.

## Synthetic fixture matrix

Generate the deterministic fixtures and render all 15 combinations (five inputs by three engines):

```sh
python3 Tools/GenerateTestRecordings.py
python3 Tools/RenderFixtureMatrix.py
```

The generator creates kick/shells, snare, toms, a synthetic room-mic signal, and synthetic overheads. Each is six seconds, 48 kHz, stereo PCM16. The matrix renderer uses the matching source profile and SPACE 65, then writes float32 outputs with a four-second tail under `recordings/rendered/`. These local audio files are git-ignored. The scripts ran successfully; every output was finite and had peak below 0 dBFS. This checks repeatability and basic signal safety, not realism or musical quality. Continue to audition with actual drum recordings when available.
