# Project Status

- **Product:** DRM-SMRTVRB
- **Company:** yeahwaah audio
- **Version:** 0.0.0 (development)
- **Phase:** 7 — Testing, optimization, and release preparation
- **Completed:** Standalone engines/profiles/SPACE; Free/Host/Tap timing calculations; five synthetic fixtures; optional advanced overrides; eight factory presets; portable versioned state codec; JUCE 8.0.6 VST3 and standalone wrapper; full Windows REAPER 7.42 host acceptance (exact dry endpoint, wet SPACE response, host tempo synchronization, automation envelopes, all eight factory programs, state/project save/reopen, UI rendering and resize down to 640 x 420).
- **Active tasks:** Release-packaging workflow merged to `main` and verified by manual dispatch (run 45210138012); audition with real drum recordings when available; resolve JUCE distribution licensing terms before release.
- **CPU:** DSP-core benchmark on macOS: worst case 0.363% of one core (48 kHz, 512 samples); Windows CI runner worst case 3.813% of one core; plugin processor (all eight presets, no editor/host) worst case 0.312% on macOS and 0.751% on the Windows CI runner; in-DAW CPU unmeasured.
- **Outstanding:** Real drum recording auditions; JUCE commercial vs. AGPLv3 distribution decision.
- **Latest successful build:** Windows x64 MSVC (Visual Studio 2022 BuildTools) + Ninja Release; VST3 and standalone built and verified locally on Windows; macOS Release also linked.
- **Latest test evidence:** Local Windows CTest passed 6/6 (ReverbEngines, TargetProfiles, SpaceMacro, TempoSync, PluginState, WavFile); Windows REAPER 7.42 passed exact dry (max error 0.0), SPACE 65 wet (delta RMS 0.09518), host tempo sync (80 vs 160 BPM delta RMS 0.00157), envelope automation, all 8 factory programs, and project save/reopen.
- **Plugin screenshots:** Verified captures of running standalone and REAPER-hosted plugin saved in `screenshots/drm-space-main.png`, `screenshots/drm-space-advanced.png`, and `screenshots/reaper-vst3-editor.png`.
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
