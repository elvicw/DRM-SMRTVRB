# Known Issues

## Modified factory programs retain their selected name

- **Observation:** One REAPER 7.34/macOS arm64 capture showed “Tight Shells” while Character was Spring, SPACE was 37.37%, and the editor said MODIFIED.
- **Expected behavior:** The editor retains the selected factory preset name when its parameters are edited and marks it MODIFIED. `isCurrentPresetModified()` compares current APVTS values with the selected preset; parameter edits do not clear the selected preset index.
- **Assessment:** Source inspection matches the observed behavior. No host state defect was established; a fresh REAPER reproduction is still useful during broader host validation.
- **Workaround:** Select a factory preset again to restore its factory values.

## Phase 6 host acceptance is incomplete

- **Completed:** Windows CI run [45163535463](https://github.com/elvicw/DRM-SMRTVRB/actions/runs/45163535463) passed configure, build, all six tests, VST3 staging, and artifact upload. REAPER 7.34/macOS checks cover SPACE output/tail, host tempo response, SPACE automation, save/reopen, and all eight factory program parameter states.
- **Still required:** Open and exercise the VST3 editor in Windows REAPER: live meters/history, resize, automation, preset/state restore, and editor reopen. CI compilation is not Windows DAW validation.
- **Additional UI evidence gap:** In the available Mac REAPER session, Play returned to stopped within one second and track peak readings stayed at zero. Offline renders processed audio but did not update the visible plugin meters/history. This leaves the live UI visualizer unverified; the observation does not establish a plugin defect.
- **Suggested fix:** Repeat the live playback/UI check in a working REAPER audio session, then complete the Windows host checklist.

## Host transport edge cases remain

- **Reproduction:** Inspect `Source/TempoSync.*` and the wrapper host playhead lookup.
- **Expected:** Host BPM changes drive synchronized timing; unavailable BPM uses fallback. Transport play/stop behavior follows the product contract.
- **Actual:** REAPER macOS render comparisons at 80 and 160 BPM produced distinct output. SPACE automation and project state/envelope restore also passed. The wrapper does not consume host play/stop state, and Tap button/UI interaction has not been validated in a live host session.
- **Suggested fix:** Confirm Tap interaction, host tempo fallback/display, and transport edge cases during the remaining host validation.

## UI acceptance checks remain

- **Completed:** Default and 640 x 420 layout review, keyboard SPACE adjustment/focus ring, accessible selector and slider names, host BPM render response, and preset parameter-state checks.
- **Outstanding:** Live meter/history visuals and full Windows REAPER UI acceptance.
- **Suggested fix:** See Phase 6 checklist in `UI_SPEC.md`.

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
