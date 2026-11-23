# Project Status

- **Product:** DRM-SMRTVRB
- **Company:** yeahwaah audio
- **Version:** 0.0.0 (development)
- **Phase:** 6 — Hardware-inspired UI
- **Completed:** Standalone engines/profiles/SPACE; Free/Host/Tap timing calculations; five synthetic fixtures; optional advanced overrides; eight factory presets; portable versioned state codec.
- **Active tasks:** Phase 6 Windows plugin and host verification; JUCE 8.0.6 is selected for implementation, while commercial/AGPL distribution terms remain undecided.
- **Outstanding:** Controlled wet/dry and host BPM/automation/state checks in REAPER; Windows REAPER validation; tune with real recordings.
- **Latest successful build:** macOS Apple clang 21.0.1, CMake 4.5.0, Ninja Release; JUCE VST3 and standalone linked after accessible control titles and responsive preset-status placement were added.
- **Latest test evidence:** Windows CI run [45053765301](https://github.com/elvicw/DRM-SMRTVRB/actions/runs/45053765301) passed all six tests on commit `782d1e7`. Local CTest has not been rerun after the editor-only changes.
- **Windows CI:** Run [45053765301](https://github.com/elvicw/DRM-SMRTVRB/actions/runs/45053765301) passed configure, build, all six tests, bundle staging, and artifact upload for the focus-ring clearance patch. Download inspection confirmed `DRM-SMRTVRB.vst3/Contents/x86_64-win/DRM-SMRTVRB.vst3` and `Contents/Resources/moduleinfo.json` are present.
- **Repository:** https://github.com/elvicw/DRM-SMRTVRB (private)

## Environment observations

- Current machine: macOS (Apple clang 21.0.1, CMake 4.5.0, Ninja available).
- Git 2.52.0 and GitHub CLI 2.104.0 are available; GitHub authentication is active for `elvicw`.
- No Windows cross-compiler was found in the initial PATH/Homebrew check.
- JUCE 8.0.6 is fetched through CMake FetchContent; clean builds require network access.
- JUCE distribution licensing remains unresolved and must be settled before binary distribution.
- REAPER 7.34/macOS arm64 scanned and loaded the VST3, opened its editor, and played `recordings/synthetic-kick-shells.wav` through the track. REAPER track and master meters showed signal. This was an exploratory smoke check, not a controlled wet/dry comparison; tempo, automation, save/reopen, plugin meters/history, and audio quality remain unverified. The unsaved test project was discarded and the temporary VST3 copy removed.
- The standalone UI was visually captured at default and 640 x 420 content sizes. Minimum-size review fixed SPACE overlap with Character and Wet Envelope caption clipping. A native mouse click plus Right arrow changed SPACE from 28% to 29%; it was restored to 28%. The SPACE control receives focus and shows an amber outline. That screenshot exposed the outline touching Character at default size, so the rotary and label were shifted together to leave a clear gap. The standalone accessibility tree reports names for preset, source, character, sync, and division selectors, plus slider names/values for SPACE, Free Decay, and advanced controls.
- The accessible-title/responsive-status editor change and handover updates are pushed in `8b15eab`.
- Offline WAV renderer is built as `drmsmrtvrb_render`; five local synthetic fixtures and fifteen profile renders are under ignored `recordings/`; no user recordings were available.
