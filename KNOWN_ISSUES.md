# Known Issues

## Modified factory programs retain their selected name

- **Observation:** One REAPER 7.34/macOS arm64 capture showed “Tight Shells” while Character was Spring, SPACE was 37.37%, and the editor said MODIFIED.
- **Expected behavior:** The editor retains the selected factory preset name when its parameters are edited and marks it MODIFIED. `isCurrentPresetModified()` compares current APVTS values with the selected preset; parameter edits do not clear the selected preset index.
- **Assessment:** Source inspection matches the observed behavior. No host state defect was established; a fresh REAPER reproduction is still useful during broader host validation.
- **Workaround:** Select a factory preset again to restore its factory values.

## Phase 6 host acceptance completed on Windows REAPER 7.42

- **Observation:** Prior validation had only covered macOS REAPER smoke checks and offline CI artifacts.
- **Verification completed (2026-11-23):** Tested natively in REAPER 7.42/win64 on Windows x64:
  - VST3 scanned, loaded, and registered in `reaper-vstplugins64.ini` without errors.
  - All 22 exposed parameters enumerated, queried, and verified.
  - All eight factory presets recalled correctly via host programs.
  - Exact dry endpoint verified: SPACE 0 render vs input has max error = `0.0000000000` (exact sample-for-sample match).
  - SPACE 65 wet render active region delta RMS = `0.09517980`.
  - Host tempo synchronization verified: renders at 80 BPM vs 160 BPM differ with delta RMS = `0.00156543`, max sample diff = `0.00897741`.
  - SPACE envelope automation (0% -> 65% -> 0%) verified with smooth wet ramp and return to zero (delta RMS 4.5e-7).
  - Project save, close, and reopen restored all parameters and envelope points accurately.
  - VST3 floating editor opened, rendered dark faceplate with amber accents, and resized to minimum 640 x 420 bounds cleanly.
  - UI screenshots captured directly from running Windows standalone and REAPER-hosted instances into `screenshots/`.
- **Environment note:** In non-interactive RDP / disconnected audio hardware sessions without an active output stream, REAPER transport play state halts immediately (`state=0`), which is host audio-engine behavior rather than a plugin defect. Offline render and full parameter/automation processing are verified.

## Host transport edge cases remain

- **Reproduction:** Inspect `Source/TempoSync.*` and the wrapper host playhead lookup.
- **Expected:** Host BPM changes drive synchronized timing; unavailable BPM uses fallback. Transport play/stop behavior follows the product contract.
- **Actual:** REAPER macOS render comparisons at 80 and 160 BPM produced distinct output. SPACE automation and project state/envelope restore also passed. The wrapper does not consume host play/stop state, and Tap button/UI interaction has not been validated in a live host session.
- **Suggested fix:** Confirm Tap interaction, host tempo fallback/display, and transport edge cases during the remaining host validation.

## UI acceptance checks completed on Windows

- **Completed:** Default and 640 x 420 layout review, keyboard SPACE adjustment/focus ring, accessible selector and slider names, host BPM render response, preset parameter-state checks, and Windows REAPER editor floating display and resizing. Real screenshots captured and saved in `screenshots/`.
- **Remaining for Phase 7:** Audition with real drum recordings when available, and settle JUCE commercial vs. AGPLv3 distribution terms.

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
