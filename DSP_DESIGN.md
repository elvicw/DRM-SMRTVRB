# DSP Design

The first core engine pass is implemented in `Source/ReverbEngines.*`. It is a development baseline, not yet tuned against real drum recordings or integrated into a plugin.

## Planned engines

- **Room:** five weighted early-reflection taps feed a four-line orthogonal feedback tank and two allpass diffusion stages.
- **Spring:** four differently timed resonant lines feed a serial dispersive allpass path for a more metallic response.
- **Gated:** short-delay dense feedback tank followed by a separately timed gate envelope; gate duration is not treated as RT60.

The engines expose decay, pre-delay, damping, width, gate duration, density, and wet level. These internal controls are not yet host parameters.

## Target-specific profiles

`Source/TargetProfiles.*` defines a distinct profile for every one of the twelve target/engine combinations. Each profile combines an input high-pass/low-pass wet-send band and send level with engine decay, pre-delay, damping, density, width, gate duration, and wet level. These values are initial settings validated for bounds and finite output; they are not claimed as final musical tuning.

The wet-send filters target source behavior: Shells reduce sub-bass excitation, Snare retains body while allowing a strong tail, Room uses lower send and tighter bandwidth, and Overheads use a narrower, more damped send to limit cymbal wash. These are processing profiles, not automatic detection.

## SPACE macro

The four user-selected sources (Shells, Snare, Room, Overheads) each have all three initial engine profiles. `Source/SpaceMacro.*` maps 0–100% through twelve per-combination curves. Smoothstep/power curves change decay, pre-delay, damping, density, width, gated duration, and wet level. The dry/wet gains use an equal-power crossfade scaled by a profile-specific wet cap. 0% is exactly dry; values ramp over 20 ms by default. Engine controls update in 32-sample quanta and mix gains update per sample without process-time allocation. These curves are initial values, not validated musical tuning. Do not treat RT60 as the audible end of a natural tail.

## Tempo synchronization calculations

`Source/TempoSync.*` converts Host, Tap, and Free modes into timing values for 1/16, 1/8, 1/4, 1/2, one bar, or two bars. A valid host/tap BPM must be 20–300; missing host tempo falls back to the last valid value or 120 BPM. Room/Spring use the division as an RT60 target, which remains a natural fade. Gated uses it as a separate termination time and reports when its four-second limit clamps a slower tempo/long division. A tempo smoother ramps timing changes. The actual host BPM adapter and transport callback are not implemented before framework selection.

## Advanced override behavior

`applyAdvancedControls` applies enabled controls after the SPACE curve. Pre-delay, Tone, Width, and Tail replace their corresponding macro-derived value; positive Tone reduces damping (brighter), negative Tone increases it (darker). Advanced Mix replaces the profile wet cap with a user wet proportion while preserving the SPACE position and exact dry output at SPACE 0%. Disabled overrides leave SPACE results unchanged. Preset/state recall uses the portable state model; the plugin wrapper must route changes through the smoothers before connecting UI automation.

## Wet filters

`Source/WetFilters.*` applies a 2nd-order Butterworth high-pass (20-1000 Hz) and low-pass (1-20 kHz) to the wet signal after the reverb engine and before the dry/wet sum, so dry audio is never filtered. A filter at its end stop (20 Hz / 20 kHz) is bypassed exactly. Cutoff changes glide in the log-frequency domain (about 20 ms) per 32-sample control quantum. Covered by the `WetFilters` CTest.

## Safety and verification

Delay buffers are prepared outside processing. Feedback is bounded, parameter inputs are clamped, and non-finite output samples are zeroed. Automated checks cover finite/bounded impulse response, engine distinction, mono routing, gated tail reduction, and generated drum-like signals at three sample rates, five block sizes, and parameter extremes. Five deterministic synthetic fixtures are also rendered through all three engines. These are procedural signals, not recordings; audio quality still needs validation with actual drum recordings and host automation.
