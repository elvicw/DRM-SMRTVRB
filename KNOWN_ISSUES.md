# Known Issues

## VST3 plugin integration remains outstanding

- **Reproduction:** Build the current CMake project and inspect its targets.
- **Expected:** A functioning Windows VST3 drum reverb plugin with musically tuned source profiles.
- **Actual:** Framework-independent DSP engines, twelve target profiles, SPACE, tempo calculations, advanced controls, factory presets, portable state codec, tests, and an offline WAV tool exist. There is no VST3 target, host integration, UI, or plugin-bound preset/state layer.
- **Workaround:** The DSP can be evaluated offline; it cannot yet be loaded by REAPER.
- **Suggested fix:** Choose the plugin framework/licensing model and continue through Phases 4–7 in ROADMAP.md.

## Host tempo and automation are not connected

- **Reproduction:** Inspect `Source/TempoSync.*` and the plugin targets in CMake.
- **Expected:** Host BPM and tempo changes drive synchronized timing in a Windows VST3.
- **Actual:** Subdivision math and fallback logic are unit-tested, but no host API adapter or VST3 wrapper exists.
- **Workaround:** Call the standalone timing API with supplied BPM values; it cannot read a DAW transport.
- **Suggested fix:** Add host extraction after framework selection and validate tempo automation in REAPER.

## DSP tuning not verified with drum recordings

- **Reproduction:** Render real kick, snare, tom, room, and overhead recordings through the engines.
- **Expected:** Useful tails without low-frequency buildup, transient loss, harsh cymbal wash, or unstable stereo image.
- **Actual:** Procedural fixtures and generated drum-like stress signals were rendered and checked for bounded finite output. They are not real recordings; musical behavior on recorded instruments remains unverified.
- **Workaround:** None established.
- **Suggested fix:** Add listening/measurement validation before calling the engines production-ready.
