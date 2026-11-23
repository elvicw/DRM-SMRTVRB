# Project Status

- **Product:** DRM-SMRTVRB
- **Company:** yeahwaah audio
- **Version:** 0.0.0 (development)
- **Phase:** 6 — Hardware-inspired UI
- **Completed:** Standalone engines/profiles/SPACE; Free/Host/Tap timing calculations; five synthetic fixtures; optional advanced overrides; eight factory presets; portable versioned state codec.
- **Active tasks:** Close Phase 6 acceptance with live meter/history and Windows REAPER UI validation; JUCE 8.0.6 is selected, while commercial/AGPL distribution terms remain undecided.
- **Outstanding:** Live meter/history visual confirmation; Windows REAPER editor/resize/automation/state validation; transport edge cases. Real-recording audition and licensing are tracked separately.
- **Latest successful build:** macOS Apple clang 21.0.1, CMake 4.5.0, Ninja Release; JUCE VST3 and standalone linked after accessible control titles and responsive preset-status placement were added.
- **Latest test evidence:** Local CTest passed 6/6 after the editor changes; Windows CI run [45163535463](https://github.com/elvicw/DRM-SMRTVRB/actions/runs/45163535463) passed configure/build/all six tests and staged the x64 VST3 artifact.
- **Latest additional signal check:** Offline Shells + Room renders of a synthetic kick matched input exactly at SPACE 0 and differed at SPACE 65 (`delta RMS=0.09518`). See `TESTING.md`; this is not host or audio-quality validation.
- **Windows CI:** Latest run [45163535463](https://github.com/elvicw/DRM-SMRTVRB/actions/runs/45163535463) passed configure, build, all six tests, bundle staging, and artifact upload. Download inspection confirmed the x64 VST3 binary and `moduleinfo.json` in the expected bundle layout.
- **Repository:** https://github.com/elvicw/DRM-SMRTVRB (private)

## Environment observations

- Current machine: macOS (Apple clang 21.0.1, CMake 4.5.0, Ninja available).
- Git 2.52.0 and GitHub CLI 2.104.0 are available; GitHub authentication is active for `elvicw`.
- No Windows cross-compiler was found in the initial PATH/Homebrew check.
- JUCE 8.0.6 is fetched through CMake FetchContent; clean builds require network access.
- JUCE distribution licensing remains unresolved and must be settled before binary distribution.
- REAPER 7.34/macOS arm64 initially scanned and loaded the VST3 and passed a basic synthetic-audio signal-through smoke check. That exploratory check did not cover controlled output, host timing, automation, or state; later controlled results are recorded below.
- REAPER 7.34/macOS: matched SPACE dry/wet/tail, host tempo response at 80 vs 160 BPM, SPACE automation, project reopen, and all eight factory program values. Live meter/history drawing remains unverified because this session’s transport stopped immediately and its track peaks stayed at zero; Windows REAPER UI acceptance also remains open. See `TESTING.md`.
- The standalone UI was visually captured at default and 640 x 420 content sizes. Minimum-size review fixed SPACE overlap with Character and Wet Envelope caption clipping. A native mouse click plus Right arrow changed SPACE from 28% to 29%; it was restored to 28%. The SPACE control receives focus and shows an amber outline. That screenshot exposed the outline touching Character at default size, so the rotary and label were shifted together to leave a clear gap. The standalone accessibility tree reports names for preset, source, character, sync, and division selectors, plus slider names/values for SPACE, Free Decay, and advanced controls.
- The accessible-title/responsive-status editor change and handover updates are pushed in `8b15eab`.
- Offline WAV renderer is built as `drmsmrtvrb_render`; five local synthetic fixtures and fifteen profile renders are under ignored `recordings/`; no user recordings were available.
