# Known Issues

## Host factory program and parameter state may disagree

- **Reproduction:** Load the VST3 in REAPER 7.34 on macOS arm64 and inspect the initial factory program and editor values.
- **Expected:** The selected “Tight Shells” program should show Shells + Room and SPACE 28%, with an unmodified status.
- **Actual:** One local capture showed “Tight Shells” as the host and editor program name, but Character was Spring, SPACE was 37.37%, and the UI said MODIFIED. The test session was not repeated, so the cause is unknown.
- **Workaround:** Select a factory preset again in the editor and confirm its values update consistently.
- **Suggested fix:** Reproduce in REAPER, then inspect JUCE program callbacks, APVTS parameter restoration, and preset status comparison; add host-level regression coverage.

## Windows VST3 host validation remains outstanding

- **Reproduction:** Build the current CMake project and inspect its targets.
- **Expected:** A functioning Windows VST3 drum reverb plugin with musically tuned source profiles.
- **Actual:** Windows CI run [45040878704](https://github.com/elvicw/DRM-SMRTVRB/actions/runs/45040878704) passed configure, build, six tests, bundle staging, and upload. The downloaded artifact contains the top-level `.vst3` bundle directory. REAPER 7.34/macOS arm64 instantiated the VST3 and opened its editor, but we did not play audio. No Windows host, automation/state round-trip, or Windows UI validation has been performed.
- **Workaround:** Use the standalone DSP renderer and tests for non-host evaluation.
- **Suggested fix:** Validate the Windows VST3 in REAPER and complete the UI acceptance checks before distribution.

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

## JUCE distribution terms are not confirmed

- **Reproduction:** Check the JUCE licence/account held by the product owner and the intended distribution model.
- **Expected:** The product is developed and distributed under terms that cover the applicable revenue and source-availability requirements.
- **Actual:** The project uses JUCE 8.0.6, but no licence status or product distribution decision is documented.
- **Workaround:** Keep binaries private and do not publish a release until the product owner verifies the terms.
- **Suggested fix:** Confirm the appropriate JUCE licence plan or AGPLv3 compliance obligations using the [official JUCE licensing overview](https://juce.com/get-juce/) and [JUCE 8 EULA](https://juce.com/legal/juce-8-licence/).
