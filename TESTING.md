# Testing

`Tests/ReverbEnginesTests.cpp` exercises Room, Spring, and Gated using stereo impulse input, checks finite/bounded output and nonzero tails, checks that the engine responses differ, checks mono processing, and checks gated tail reduction after its selected gate interval.

Latest recorded automated result: Windows CI run [45185893939](https://github.com/elvicw/DRM-SMRTVRB/actions/runs/45185893939) passed all six CTest targets (`ReverbEngines`, `TargetProfiles`, `SpaceMacro`, `TempoSync`, `PluginState`, `WavFile`) and staged/uploaded the VST3 artifact. In addition, native Windows REAPER 7.42 validation was performed and verified locally on Windows x64.

The first Windows CI run compiled successfully but the test executable segfaulted because the harness stored multiple full render buffers on the Windows thread stack. Moving these buffers to heap-backed vectors fixed the issue. Windows run 45004120337 passed configure, build, and both CTest targets on `windows-latest`.

Remaining coverage includes transport edge cases, plugin meter/history behavior in a DAW, long runs, CPU/memory behavior, actual drum recordings, and final REAPER validation on Windows.

## Controlled offline SPACE endpoint check (2026-11-23)

Rendered `recordings/synthetic-kick-shells.wav` through the Shells + Room profile at SPACE 0 and SPACE 65 using the existing `build/drmsmrtvrb_render` executable. SPACE 0 matched the input sample-for-sample over the 6-second input (`max_input_error=0`). SPACE 65 differed from the dry output (`delta RMS=0.0951797994`). Both outputs were stereo, 48 kHz, 10 seconds including the renderer's four-second tail, and remained below 0 dBFS. The generated files are ignored under `build-plugin/controlled-space-{0,65}.wav`.

This verifies the offline renderer's exact dry endpoint and nonzero SPACE processing on one synthetic fixture only. It does not establish wet audio quality or host behavior.

## Phase 1 stress checks

The automated stress test renders repeated drum-like hits and deterministic noise across 44.1, 48, and 96 kHz; block sizes 32, 64, 128, 256, and 1024; all three engines; and alternating parameter extremes: decay 0.08/8 s, pre-delay 0/200 ms, damping 0/1, width 0/2, gate 0.03/4 s, density 0/1, and wet level 0/2. It checks that samples remain finite and peaks bounded.

`TargetProfiles` checks all twelve distinct source/engine profiles for valid ranges, finite/bounded wet output, and mono/stereo processing. `SpaceMacro` checks all twelve curve endpoints and bounds, 20 ms smoothing, exact dry output at zero, and wet output after automation. The render matrix uses each source profile and SPACE 65 for the synthetic source fixtures.

`TempoSync` checks all six subdivision calculations, BPM changes and missing-host fallback, Free mode, Tap mode fallback, tap averaging/outlier behavior, natural decay vs gated termination limits, and a 25 ms timing ramp. It does not test host transport extraction because no plugin framework/wrapper exists yet.

`PluginState` checks advanced overrides and their interaction with SPACE, all eight preset names/states, version-1 state round trips, unknown-field forward compatibility, CRLF input, and fail-without-partial-mutation for unsupported state versions.

Numerical guards cover non-finite sample rates, SPACE inputs, and audio samples; invalid control values fall back to safe settings.

## Listening with drum recordings

The `drmsmrtvrb_render` utility accepts mono/stereo PCM16, PCM24, PCM32, or float32 WAV. It produces wet-only float32 WAV and appends four seconds by default. Example from the repository root:

```sh
./build/drmsmrtvrb_render --engine room \
  --input recordings/snare.wav --output build/snare-room-wet.wav \
  --decay 1.1 --predelay 8 --damping 0.45 --width 1 --tail 4
```

Render all three engines on kick/shells, snare, tom, room-mic, and overhead recordings. Listen to dry and wet at matched playback level; check kick low-end buildup, snare transient definition, cymbal wash, stereo stability, and gated tail timing. Manual engine renders are wet-only; `--target ... --space N` exercises the profile and SPACE crossfade. Private source files belong in the git-ignored `recordings/` folder.

No actual drum recordings were available in the workspace. Listening validation with real material remains outstanding; record the source, settings, observations, and any problematic samples here before marking Phase 1 complete.

## Synthetic fixture matrix

Generate the deterministic fixtures and render all 15 combinations (five inputs by three engines):

```sh
python3 Tools/GenerateTestRecordings.py
python3 Tools/RenderFixtureMatrix.py
```

The generator creates kick/shells, snare, toms, a synthetic room-mic signal, and synthetic overheads. Each is six seconds, 48 kHz, stereo PCM16. The matrix renderer uses the matching source profile and SPACE 65, then writes float32 outputs with a four-second tail under `recordings/rendered/`. These local audio files are git-ignored. The scripts ran successfully; every output was finite and had peak below 0 dBFS. This checks repeatability and basic signal safety, not realism or musical quality. Continue to audition with actual drum recordings when available.


## Controlled REAPER SPACE comparison (2026-11-23)

REAPER 7.34/macOS arm64 instantiated the VST3 in a saved, controlled project with the synthetic kick fixture. At SPACE 0 the six-second rendered region matched the source sample-for-sample (maximum error 0). At SPACE 65 (host normalized value 0.65), output differed from dry (`delta RMS=0.09521270 FS`; active-region dry RMS 0.19502668 FS, wet RMS 0.10314160 FS). Both were stereo 48 kHz PCM16 and remained below 0 dBFS.

The first host run appeared to have no tail, but its state did not match the offline render: the plugin constructor loads the Tight Shells factory preset, which enables an advanced 0.48 s tail override. The offline renderer used Shells + Room profile defaults without that override. With the advanced override toggles disabled in REAPER and a ten-second source containing four seconds of silence, the SPACE 65 host output matched the offline renderer within one PCM16 quantization step (`max error=1.53e-5 FS`) from 0 through 7 seconds. At 6 seconds the host tail had peak 0.0002136 FS and RMS 0.00003197 FS; the remaining tail fell below PCM16 quantization by 7 seconds. SPACE 0 remained sample-identical to input. This confirms the DSP tail and VST3 host processing; the earlier apparent failure was caused by different advanced tail settings.

The default Tight Shells preset still uses its intentional 0.48 s tail override, so tail length depends on preset state. This comparison verifies signal behavior and the dry endpoint on one synthetic fixture; it does not establish subjective audio quality. Outputs and test projects are ignored under `build-plugin/`.


## REAPER host tempo, automation, and state round trip (2026-11-23)

In REAPER 7.34/macOS arm64, the same ten-second synthetic source was rendered with Host sync, quarter-note division, SPACE 65, advanced overrides disabled, and the media item fixed to time. Project tempo markers were set to 80 and 160 BPM. The renders differed (`delta RMS=0.00121261 FS`, max sample difference `0.00897217 FS`); the 80 BPM render retained measurable tail energy into 6-7 seconds while the 160 BPM tail had fallen below PCM16 quantization by 6 seconds. This verifies that the plugin responds to host tempo changes in this setup.

A SPACE FX envelope was created with points at 0 s = 0%, 3 s = 65%, and 5.5 s = 0%, using linear segments. The render changed from near-dry before the ramp to a clear wet contribution during 3.2-4.8 s (`delta RMS from dry=0.07960 FS`). The saved project was reopened. REAPER restored the Room target (`normalized=0.6667`), Spring character (`normalized=0.5`), and all three SPACE envelope points. SPACE reads 0 at the reopened project cursor because the envelope's first point is 0%.

These are synthetic macOS REAPER checks. They do not cover transport edge cases, all parameter envelopes, Windows REAPER, or subjective audio quality. Test projects and WAVs are ignored under `build-plugin/`.

## Phase 6 verification addendum (2026-11-23)

The local macOS build was current (`cmake --build build-plugin`: no work); CTest passed all six targets. A REAPER 7.34 host program check selected all eight factory programs and matched their displayed names plus normalized Target, Character, SPACE, Sync, and Division values against `Source/PluginState.cpp`.

Windows workflow run [45163535463](https://github.com/elvicw/DRM-SMRTVRB/actions/runs/45163535463) passed configure, build, all six tests, VST3 bundle staging, and artifact upload. Artifact inspection confirmed `DRM-SMRTVRB.vst3/Contents/x86_64-win/DRM-SMRTVRB.vst3` and `Contents/Resources/moduleinfo.json`. This does not verify the plugin inside Windows REAPER.

## Controlled Windows REAPER 7.42 Verification (2026-11-23)

Native testing on Windows x64 using REAPER 7.42/x64 and the locally compiled Windows VST3 (`DRM-SMRTVRB.vst3`):

1. **Scan and Load:** REAPER scanned `%LOCALAPPDATA%\Programs\Common\VST3` and `C:\Program Files\Common Files\VST3`, registering `DRM-SMRTVRB (yeahwaah audio)` in `reaper-vstplugins64.ini`. The plugin loaded cleanly into a track FX chain. All 22 exposed parameters were queried and matched APVTS definitions.
2. **Factory Programs:** All eight factory presets were recalled sequentially via the host Program parameter (`Param 18`) and confirmed to match `Source/PluginState.cpp`:
   - Tight Shells (Shells, Room, SPACE 28%, Free, 1/4, Tail 0.48 s)
   - Big Snare (Snare, Room, SPACE 76%, Host, 1/2, Tail 2.10 s)
   - Explosive Snare (Snare, Gated, SPACE 86%, Tap, 1/4, Tail 0.32 s)
   - Small Studio (Room, Room, SPACE 32%, Free, 1/4, Tail 0.62 s)
   - Large Room (Room, Room, SPACE 74%, Host, 1 bar, Tail 2.00 s)
   - Wide Overheads (Overheads, Room, SPACE 48%, Free, 1/4, Tail 0.85 s)
   - Metallic Spring (Snare, Spring, SPACE 61%, Free, 1/4, Tail 1.60 s)
   - Vintage Gated (Snare, Gated, SPACE 73%, Tap, 1/4, Tail 0.30 s)
3. **Exact Dry Endpoint (SPACE 0%):** Rendered `recordings/synthetic-kick-shells.wav` through REAPER with advanced overrides disabled at SPACE 0. Compared to dry source over all 288,000 frames (6.0 s), the maximum absolute sample error was `0.0000000000` (exact sample-for-sample pass-through).
4. **Wet Reverb Processing (SPACE 65%):** Rendered the same fixture at SPACE 65% with profile defaults. Active-region delta RMS was `0.09517980`. Output signal was finite, well-behaved, and peaked at 0.512281 (below 0 dBFS).
5. **Host Tempo Synchronization:** Rendered at 80 BPM vs 160 BPM with Host Sync and quarter-note division. The renders differed measurably with delta RMS `0.00156543` and max sample difference `0.00897741`, confirming active host tempo tracking.
6. **Automation Envelope:** Automated SPACE envelope with points at 0.0 s = 0%, 3.0 s = 65%, and 5.5 s = 0%. The render showed dry audio at the start (diff RMS 0.0044), strong wet contribution at the peak (diff RMS 0.0628), and clean return to zero at 5.5–6.0 s (diff RMS 0.00000045).
7. **Project Save/Reopen Round-Trip:** Saved project with customized parameters (Snare + Spring, SPACE 65%, advMixOn, advMix 80%, and 3-point SPACE envelope). Reopened project from disk; all parameters, preset modified state, and envelope points were restored identically.
8. **UI Editor and Resize:** Standalone and hosted VST3 editor windows opened with the dark faceplate and amber illumination. Resized to minimum size (640 x 420) without visual overlap or clipping. Captured real running screenshots into `screenshots/drm-space-main.png`, `screenshots/drm-space-advanced.png`, and `screenshots/reaper-vst3-editor.png`.

## CPU benchmark (DSP only)

`drmsmrtvrb_benchmark [sampleRate=48000] [blockSize=512] [seconds=30]` runs `SpaceMacroProcessor` (SPACE 65, stereo) for every source target and engine with a 100 ms noise burst each second, so decaying tails are included, and reports CPU as a share of one core. It exits non-zero on non-finite output. Build: `cmake -S . -B build -G Ninja -DCMAKE_BUILD_TYPE=Release && cmake --build build`.

Observed on the macOS development machine (Apple silicon, Release, 2026-11-23): 48 kHz / 512-sample blocks worst case 0.363% of one core (Room engine, Shells target; other combinations 0.18–0.25%); 44.1 kHz / 64-sample blocks worst case 0.204%; all output finite. Scope: DSP core only, not the JUCE wrapper, editor/meters, a DAW, or Windows hardware. Windows runner (GitHub `windows-latest`, MSVC Release, 48 kHz/512, run [45210114347](https://github.com/elvicw/DRM-SMRTVRB/actions/runs/45210114347)): worst case 3.813% of one core (overheads/room engine), range 1.49–3.81%, all finite; shared CI hardware, so treat as an upper-bound indication. The Windows workflow now runs this benchmark informationally. Plugin-in-host CPU remains unmeasured.

## CPU benchmark (plugin processor)

`drmsmrtvrb_plugin_benchmark [sampleRate=48000] [blockSize=512] [seconds=30]` (built with `-DDRMSMRTVRB_BUILD_PLUGIN=ON`) drives the real `DRMsmrtvrbAudioProcessor::processBlock` for each of the eight factory presets with the same noise-burst-plus-tail signal, so parameter reads, tempo sync (no host playhead, fallback BPM), smoothing, meters and activity history are included. Output: `build-plugin/drmsmrtvrb_plugin_benchmark_artefacts/Release/drmsmrtvrb_plugin_benchmark`. Exits non-zero on non-finite output.

Observed on macOS (Apple silicon, Release, 2026-11-24): 48 kHz / 512 worst case 0.312% of one core (Tight Shells; others 0.245–0.284%); 44.1 kHz / 64 worst case 0.309%; all finite. Still excludes the editor/UI drawing, a real DAW host, and live host-playhead lookup. Windows runner (`windows-latest`, MSVC Release, 48 kHz/512, run [45212922040](https://github.com/elvicw/DRM-SMRTVRB/actions/runs/45212922040)): worst case 0.751% of one core (Small Studio; range 0.617–0.751%), all finite; shared CI hardware.

## Stability soak (plugin processor)

`drmsmrtvrb_soak_test [secondsPerConfig=20] [seed=12345]` (built with `-DDRMSMRTVRB_BUILD_PLUGIN=ON`; CTest `PluginSoak` runs it with 10 s per config) drives `DRMsmrtvrbAudioProcessor` at 22.05/44.1/48/88.2/96/192 kHz in stereo and mono with random block sizes (1–4096), random changes to every exposed parameter (biased to range ends), preset switching, repeated `prepareToPlay` with new maximum block sizes, and silent/noise/sine/full-scale-DC/burst input. It fails on non-finite output, output peak above 16, or (POSIX only) more than 16 MB resident-memory growth over 200 re-prepare cycles at 192 kHz after warm-up. It does not check audio quality, and the memory check does not run on Windows.

Observed on macOS (Apple silicon, Release, 2026-11-24): default 10 s/config (120 s of audio): PASS, peak 1.162, leak-phase growth 0 kB; 120 s/config with seed 777 (1,440 s of audio): PASS, peak 1.484, growth 0 kB. Wall time about 1.6 s for the default run. This covers processor-level behavior only, not a DAW host, the editor, or real-time scheduling.
