# Known Issues

## Host factory program and parameter state may disagree

- **Reproduction:** Load the VST3 in REAPER 7.34 on macOS arm64 and inspect the initial factory program and editor values.
- **Expected:** The selected “Tight Shells” program should show Shells + Room and SPACE 28%, with an unmodified status.
- **Actual:** One local capture showed “Tight Shells” as the host and editor program name, but Character was Spring, SPACE was 37.37%, and the UI said MODIFIED. The test session was not repeated, so the cause is unknown.
- **Workaround:** Select a factory preset again in the editor and confirm its values update consistently.
- **Suggested fix:** Reproduce in REAPER, then inspect JUCE program callbacks, APVTS parameter restoration, and preset status comparison; add host-level regression coverage.

## Windows VST3 and REAPER validation remain outstanding

- **Reproduction:** Build the current CMake project and inspect its targets.
- **Expected:** A functioning Windows VST3 drum reverb plugin with musically tuned source profiles.
- **Actual:** The current JUCE VST3 and standalone compile locally, and REAPER 7.34/macOS arm64 instantiated the VST3 and opened its editor. We did not play audio. No Windows build, automation/state round-trip, or Windows host UI validation has been performed.
- **Workaround:** Use the standalone DSP renderer and tests for non-host evaluation.
- **Suggested fix:** Build on Windows, validate in REAPER, and complete the UI acceptance checks before distribution.

## Host transport behavior and full automation validation remain outstanding

- **Reproduction:** Inspect `Source/TempoSync.*` and the plugin targets in CMake.
- **Expected:** Host BPM and tempo changes drive synchronized timing in a Windows VST3.
- **Actual:** The wrapper reads BPM when the host exposes it and uses the timing module, but it does not consume transport play/stop state. Tempo automation, host parameter automation, and state restoration have not been exercised in a DAW.
- **Workaround:** Free and Tap modes remain available; missing BPM uses the timing module fallback.
- **Suggested fix:** Validate host tempo changes, transport behavior, automation, and host state restore in REAPER.

## UI acceptance checks remain

- **Reproduction:** Open the editor and inspect the meter strip and preset selector.
- **Expected:** Host tempo display, advanced values enabled only when their toggles are active, and a responsive, accessible editor.
- **Actual:** The BPM display and toggle-based control enabling are implemented and compile. The default editor size was visually checked; Host/Tap modes, minimum-size layout, keyboard/accessibility behavior, meter activity, and live wet history have not been verified.
- **Workaround:** Numeric parameters and host controls remain available.
- **Suggested fix:** Complete the UI meter handoff and interaction requirements in `UI_SPEC.md`.

## DSP tuning not verified with drum recordings

- **Reproduction:** Render real kick, snare, tom, room, and overhead recordings through the engines.
- **Expected:** Useful tails without low-frequency buildup, transient loss, harsh cymbal wash, or unstable stereo image.
- **Actual:** Procedural fixtures and generated drum-like stress signals were rendered and checked for bounded finite output. They are not real recordings; musical behavior on recorded instruments remains unverified.
- **Workaround:** None established.
- **Suggested fix:** Add listening/measurement validation before calling the engines production-ready.
