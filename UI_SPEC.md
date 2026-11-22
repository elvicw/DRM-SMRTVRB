# DRM-SMRTVRB Interface Specification

## Status

Phase 6 design with an initial JUCE editor implementation in `Source/PluginEditor.*`. The editor binds to actual APVTS parameters. JUCE 8.0.6 is selected for implementation; distribution license terms remain to be resolved before release.

## Main window

- Default content size: 760 x 500 logical pixels; keep all controls usable at 640 x 420.
- Dark charcoal faceplate, restrained brushed-metal detail, warm amber active accents, high-contrast labels, and compact studio-hardware styling.
- Use a clear top-to-bottom hierarchy: preset and identity, source/character choices, SPACE, timing, then optional advanced controls and meters.
- All controls remain visible without scrolling at the default size. Advanced controls may open a compact side panel or expand below the main controls at the minimum size.

## Layout and behavior

| Region | Controls / display | Required behavior |
|---|---|---|
| Header | DRM-SMRTVRB, yeahwaah audio, preset selector | Preset selection recalls the complete versioned state; modified state is indicated until saved or another preset is recalled. |
| Source | Shells, Snare, Room, Overheads | Exclusive selection; changing target updates the active profile and is host-automatable. |
| Character | Room, Spring, Gated | Exclusive selection; changing character updates the active profile and is host-automatable. |
| Main | Large SPACE rotary, numeric percent readout | Dominant control; 0% is exact dry, 100% reaches the profile's intended maximum. Drag, mouse wheel, keyboard arrows, and double-click default are supported. |
| Timing | Free / Host / Tap, BPM readout, division selector, tap button | Host mode displays host tempo or the documented fallback; Tap measures taps; divisions are enabled only when relevant. Gated tail timing follows the selected division and the engine's safe maximum. |
| Advanced | Pre-delay, Tone, Width, Mix, Tail; each with enable/bypass | Values edit only when enabled. Disabled values remain stored. Engine/profile constraints are shown in the value range and respected by DSP. |
| Meter strip | Input and output peak meters | Show stereo channels independently, with peak hold and decay; meter handoff must be lock-free and safe for the audio callback. |
| Reverb activity | Compact envelope/history display | Visualize the recent wet output envelope from audio-thread data; provide a static reduced-motion fallback and no audio-thread drawing or allocation. |

## Interaction and accessibility

- Every control has a persistent text label, readable value, keyboard focus, and accessible name/role/value through the chosen framework.
- Use distinct selected states beyond color alone; maintain text/background contrast and visible focus rings.
- Rotary controls support click-drag, fine adjustment with a modifier, wheel adjustment, arrow-key adjustment, and double-click reset to the documented default.
- Preset changes, source/character changes, and advanced enable switches are undoable through normal host automation/state recall behavior where supported.
- Avoid modal dialogs for routine parameter editing. Tooltips explain the audible purpose in plain language.
- Reduce animation when the operating system requests reduced motion; meters must not be the only indication of signal presence.

## Parameter and state contract

- Stable host parameter IDs are assigned before UI implementation and remain stable across releases.
- UI reads and writes the same parameter/state model used by the processor; it does not keep a second authoritative copy.
- Target, character, SPACE, sync mode, division, free decay, and advanced values/enable flags persist through host state.
- Host BPM and transport are read-only displays supplied by the processor; Tap BPM remains editable only in Tap mode.
- Preset labels map to the eight existing factory states. User presets, preset file browsing, and preset deletion are outside the initial V1 UI unless the roadmap is revised.
- The editor polls or receives copied meter/envelope snapshots at a bounded UI rate. It never calls DSP processing methods or touches mutable audio buffers.

## Acceptance checks for implementation

1. All 12 target/character choices select the corresponding processor profile.
2. SPACE updates audible processing continuously, gives exact dry at zero, and stays synchronized when restored from host state.
3. Free/Host/Tap and division controls reflect valid BPM/timing and graceful missing-host-tempo fallback.
4. Each advanced control is disabled by default unless the state says otherwise; toggles and values survive save/restore.
5. All eight factory presets recall the exact portable state and update every visible control.
6. Input/output meters and reverb activity show live audio without UI access to audio-thread buffers.
7. Controls are operable by mouse and keyboard at the default and minimum window sizes.
8. Validate in a Windows VST3 host, including resizing, automation, preset/state restore, and editor reopen.

## Framework-dependent work remaining

The editor implements the main selectors, SPACE, timing controls, advanced overrides, factory preset choice, stereo meter/activity history, and preset modified indication. The standalone window has been captured at default size and at 640 x 420 content size. The minimum-size review found SPACE overlapping Character and the Wet Envelope caption reaching the bottom edge; both defects were corrected, and the revised capture shows the controls clearly. Accessibility inspection reports names and values for Source, Character, Tempo Sync, SPACE, Free Decay, and advanced sliders. Sliders request keyboard focus and draw an amber outline while focused; runtime keyboard operation remains to be verified. Preset/division selector accessibility, Host BPM and advanced-toggle behavior, and Windows host validation remain open. Keep the DSP model unchanged if framework-specific details differ.
