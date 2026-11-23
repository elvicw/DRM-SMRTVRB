# Testing

`Tests/ReverbEnginesTests.cpp` exercises Room, Spring, and Gated using stereo impulse input, checks finite/bounded output and nonzero tails, checks that the engine responses differ, checks mono processing, and checks gated tail reduction after its selected gate interval.

Latest recorded automated result: Windows CI run [45163535463](https://github.com/elvicw/DRM-SMRTVRB/actions/runs/45163535463) passed all six CTest targets (`ReverbEngines`, `TargetProfiles`, `SpaceMacro`, `TempoSync`, `PluginState`, `WavFile`) and staged/uploaded the VST3 artifact. The macOS standalone UI also passed the documented minimum-size and keyboard/accessibility checks. These checks do not replace DAW behavior or audio-quality validation.

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

The live meter/history editor check remains open. The available REAPER session stopped transport within one second and track peak readings stayed at zero; offline renders produced audio but did not update the visible plugin meter/history window. No conclusion about a product defect is drawn from that environment result. The remaining Phase 6 checks are live meter/history behavior in a working playback session and Windows REAPER editor resize, automation, preset/state restoration, and reopen behavior.
