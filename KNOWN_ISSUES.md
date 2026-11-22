# Known Issues

## Plugin integration and listening validation remain outstanding

- **Reproduction:** Open the repository and inspect the source tree.
- **Reproduction:** Build the current CMake project and inspect its targets.
- **Expected:** A functioning Windows VST3 drum reverb plugin with musically tuned source profiles.
- **Actual:** Only a framework-independent DSP library and impulse tests exist. There is no VST3 target, host integration, source profiling, SPACE macro, UI, or preset layer.
- **Workaround:** The DSP engines can be built and exercised with CMake; they cannot yet be loaded by REAPER.
- **Suggested fix:** Complete engine validation, choose the plugin framework/licensing model, and continue through Phases 2–7 in ROADMAP.md.

## DSP tuning not verified with drum recordings

- **Reproduction:** Render real kick, snare, tom, room, and overhead recordings through the engines.
- **Expected:** Useful tails without low-frequency buildup, transient loss, harsh cymbal wash, or unstable stereo image.
- **Actual:** Only synthetic impulse tests have been run; musical behavior is unverified.
- **Workaround:** None established.
- **Suggested fix:** Add listening/measurement validation before calling the engines production-ready.
