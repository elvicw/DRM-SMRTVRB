# Known Issues

## Modified factory programs retain their selected name

- **Observation:** One REAPER 7.34/macOS arm64 capture showed “Tight Shells” while Character was Spring, SPACE was 37.37%, and the editor said MODIFIED.
- **Expected behavior:** The editor retains the selected factory preset name when its parameters are edited and marks it MODIFIED. `isCurrentPresetModified()` compares current APVTS values with the selected preset; parameter edits do not clear the selected preset index.
- **Assessment:** Source inspection matches the observed behavior. No host state defect was established; a fresh REAPER reproduction is still useful during broader host validation.
- **Workaround:** Select a factory preset again to restore its factory values.

## Windows VST3 host validation remains outstanding

- **Reproduction:** Build the current CMake project and inspect its targets.
- **Expected:** A functioning Windows VST3 drum reverb plugin with musically tuned source profiles.
- **Actual:** Windows CI run [45053765301](https://github.com/elvicw/DRM-SMRTVRB/actions/runs/45053765301) passed configure, build, six tests, bundle staging, and upload for the focus-ring clearance patch. The downloaded artifact contains the top-level `.vst3` bundle directory. REAPER 7.34/macOS arm64 instantiated the VST3 and opened its editor, but we did not play audio. No Windows host, automation/state round-trip, or Windows UI validation has been performed.
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
- **Actual:** The BPM display and toggle-based control enabling are implemented. Default-size and 640 x 420 content-size standalone captures show the layout; the minimum-size review fixed SPACE overlapping Character and Wet Envelope caption clipping. A native click and Right arrow moved SPACE from 28% to 29%, then it was restored to 28%; the amber focus outline was visible. Its border touched Character at default size, so the rotary and label were shifted together to leave a clear gap. The accessibility tree reports names for preset, source, character, sync, and division selectors, plus slider names/values for SPACE, Free Decay, and advanced controls. Host/Tap behavior, meter activity, and wet history remain unverified.
- **Workaround:** Numeric parameters and host controls remain available.
- **Suggested fix:** Verify meter handoff and Host/Tap modes in a host and standalone session.

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
