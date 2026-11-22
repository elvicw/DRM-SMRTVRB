# Testing

`Tests/ReverbEnginesTests.cpp` exercises Room, Spring, and Gated using stereo impulse input, checks finite/bounded output and nonzero tails, checks that the engine responses differ, checks mono processing, and checks gated tail reduction after its selected gate interval.

Latest local result: CMake Release build succeeded on macOS with Apple clang 21.0.1; CTest passed 1/1 (`ReverbEngines`). This is DSP unit coverage only, not Windows plugin or DAW validation.

Planned coverage includes mono/stereo routing, all twelve source/engine combinations, SPACE range and transitions, tempo sync and missing tempo, transport and automation changes, preset/state restoration, sample-rate and block-size changes, long runs, CPU/memory behavior, and real drum recordings. Final host validation is required in REAPER on Windows.
