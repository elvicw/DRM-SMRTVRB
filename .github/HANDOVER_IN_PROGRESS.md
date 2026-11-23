# Windows REAPER Handoff — DRM-SMRTVRB (2026-11-23)

## Purpose

Finish the Windows host portion of Phase 6. A Windows machine with REAPER is needed to verify that the Windows VST3 loads and that its editor works during live playback. macOS REAPER checks and Windows CI are useful evidence, but do not substitute for this host check.

## Current project state

- Repository: [elvicw/DRM-SMRTVRB](https://github.com/elvicw/DRM-SMRTVRB), branch `main`.
- Latest commit: `3d4eae3` (`docs: record phase 6 verification`), pushed to `origin/main`; working tree was clean apart from this handoff file.
- Windows CI run [45163535463](https://github.com/elvicw/DRM-SMRTVRB/actions/runs/45163535463) passed configure, build, all six CTest targets, VST3 staging, and artifact upload. The x64 VST3 bundle and `moduleinfo.json` were inspected after download.
- macOS REAPER 7.34 checks passed for SPACE dry/wet, host tempo response, SPACE automation, project save/reopen, and all eight factory program values. These are macOS host results.
- macOS live meter/history drawing remains unverified: the available REAPER transport stopped immediately and reported no track peaks, so this did not establish a plugin defect.
- Phase 6 remains open for live meter/history UI behavior and Windows REAPER editor/resize/automation/state/reopen validation.

## Windows tester setup

1. On a Windows x64 machine, open the CI run above and download the `DRM-SMRTVRB-Windows-VST3` artifact from its **Artifacts** section. The GitHub account needs access to this private repository.
2. Extract the artifact. Copy the complete `DRM-SMRTVRB.vst3` bundle into the user's VST3 folder, normally `%LOCALAPPDATA%\\Programs\\Common\\VST3` or `C:\\Program Files\\Common Files\\VST3`. Preserve the bundle directory structure.
3. Start REAPER, open **Options → Preferences → Plug-ins → VST**, add the install folder if needed, and run **Re-scan**. Confirm `DRM-SMRTVRB` appears and loads.
4. Create a short audio item with a drum hit or loop. If no test audio is handy, clone the repository and create the included synthetic kick fixture with `python Tools/GenerateTestRecordings.py`; import `recordings/synthetic-kick-shells.wav` into REAPER. This fixture is generated locally and is not committed.
5. Insert the plugin on the track, select a working audio output in **Options → Preferences → Audio → Device**, and play in real time. Confirm REAPER's transport and track/master meters move before judging plugin live visuals.

## Checks to perform

- **Load/editor:** Plugin scans, opens, and displays correctly.
- **Live visuals:** With audio playing, confirm stereo meters respond and decay, and the wet history updates. Note whether the display changes with SPACE at 0% versus a clearly wet value such as 65%.
- **Resize:** Resize the editor down to its minimum and back up; check for clipped or overlapping controls. Try keyboard focus/adjustment on SPACE if practical.
- **Automation:** Add a SPACE envelope that moves from 0% to 65% and back while playing; confirm both the sound and displayed value follow it.
- **Preset/state:** Select another factory preset, save the project, close/reopen it, and confirm the selected preset and parameter values restore. Reopen the editor and confirm it remains correct.

Use a real-time audio device for the live-visual check; an offline render does not establish live editor updates.

## Report back

Record Windows version, REAPER version, plugin artifact/CI run used, audio device, and pass/fail for each check. Include exact failure steps and a screenshot of the plugin editor plus visible REAPER transport/meters for any issue. Keep the result explicitly scoped to the tested Windows and REAPER versions.

## Evidence and project notes

- Detailed test evidence and limits: `TESTING.md`.
- Current phase summary: `PROJECT_STATUS.md`.
- Build/artifact background: `BUILD_INSTRUCTIONS.md` and `AGENT_HANDOVER.md`.
- The Windows CI artifact validates the build and package layout; it does not prove Windows REAPER behavior.
- Real drum-recording audition and the JUCE distribution license decision are separate outstanding project items.
