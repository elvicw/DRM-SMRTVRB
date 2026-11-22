# Project Status

- **Product:** DRM-SMRTVRB
- **Company:** yeahwaah audio
- **Version:** 0.0.0 (development)
- **Phase:** 6 — Hardware-inspired UI
- **Completed:** Standalone engines/profiles/SPACE; Free/Host/Tap timing calculations; five synthetic fixtures; optional advanced overrides; eight factory presets; portable versioned state codec.
- **Active tasks:** Phase 6 Windows plugin and host verification; JUCE 8.0.6 is selected for implementation, while commercial/AGPL distribution terms remain undecided.
- **Outstanding:** Test REAPER audio/automation/state/tempo; tune with real recordings.
- **Latest successful build:** macOS Apple clang 21.0.1, CMake 4.5.0, Ninja Release; JUCE VST3 and standalone linked after accessible control titles and responsive preset-status placement were added.
- **Latest successful test:** `ctest --test-dir build-plugin --output-on-failure` passed 6/6 before the latest editor-only accessibility/layout patch; it has not been rerun after that patch.
- **Windows CI:** Run [45052266119](https://github.com/elvicw/DRM-SMRTVRB/actions/runs/45052266119) passed configure, build, all six tests, bundle staging, and artifact upload for the minimum-size/focus patch. Download inspection confirmed `DRM-SMRTVRB.vst3/Contents/x86_64-win/DRM-SMRTVRB.vst3` and `Contents/Resources/moduleinfo.json` are present.
- **Repository:** https://github.com/elvicw/DRM-SMRTVRB (private)

## Environment observations

- Current machine: macOS (Apple clang 21.0.1, CMake 4.5.0, Ninja available).
- Git 2.52.0 and GitHub CLI 2.104.0 are available; GitHub authentication is active for `elvicw`.
- No Windows cross-compiler was found in the initial PATH/Homebrew check.
- JUCE 8.0.6 is fetched through CMake FetchContent; clean builds require network access.
- JUCE distribution licensing remains unresolved and must be settled before binary distribution.
- Local REAPER 7.34 (macOS arm64) scanned and instantiated the VST3 and opened its editor. No audio was played. One capture showed “Tight Shells” with different parameter values and a MODIFIED indicator. Source inspection confirms the editor intentionally retains the selected factory preset while flagging parameter differences; no defect was established. Temporary install was removed and REAPER was closed.
- The standalone UI was visually captured at default and 640 x 420 content sizes. Minimum-size review fixed SPACE overlap with Character and Wet Envelope caption clipping. A native mouse click plus Right arrow changed SPACE from 28% to 29%; it was restored to 28%. The SPACE control receives focus and shows an amber outline. That screenshot exposed the outline touching Character at default size, so the rotary and label were shifted together to leave a clear gap. The standalone accessibility tree reports names for preset, source, character, sync, and division selectors, plus slider names/values for SPACE, Free Decay, and advanced controls.
- The accessible-title/responsive-status editor change and handover updates are pushed in `8b15eab`.
- Offline WAV renderer is built as `drmsmrtvrb_render`; five local synthetic fixtures and fifteen profile renders are under ignored `recordings/`; no user recordings were available.
