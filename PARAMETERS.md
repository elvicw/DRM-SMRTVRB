# Parameters

No host-automatable parameters are implemented yet. The values below are internal DSP/profile settings; they do not have plugin IDs, automation, or saved-state behavior yet. The values are initial engineering defaults and remain provisional pending listening checks with real drum recordings.

## Internal controls

| Name | Type/range | Function |
|---|---|---|
| Decay | seconds, 0.08–8.0 | Reverb feedback decay |
| Pre-delay | ms, 0–200 | Delay before wet excitation |
| Damping | 0–1 | Frequency-dependent decay damping |
| Width | 0–2 | Stereo side gain |
| Gate | seconds, 0.03–4.0 | Gated engine termination |
| Density | 0–1 | Engine-specific diffusion/feedback density |
| Wet level | 0–2 | Wet output gain |
| Send high-pass / low-pass | Hz | Source-specific band limiting before reverb |
| Send level | linear, 0–1 | Source-specific wet excitation |
| SPACE | percent, 0–100 | Per-profile macro for decay, pre-delay, damping, density, width, gate, and wet/dry gains |
| SPACE smoothing | ms, 1–100; default 20 | Standalone macro processor transition ramp |
| Sync mode | Free / Host / Tap | Selects unrestricted, host BPM, or tapped BPM timing |
| BPM | 20–300 | Host/tap musical timing; missing source falls back to last valid BPM or 120 |
| Division | 1/16, 1/8, 1/4, 1/2, 1 bar, 2 bars | Converts BPM to timing milliseconds |
| Advanced Pre-delay | ms, 0–200; optional | Overrides the macro pre-delay when enabled |
| Advanced Tone | -1–1; optional | Dark-to-bright damping offset |
| Advanced Width | 0–2; optional | Overrides macro stereo width |
| Advanced Mix | 0–1; optional | Overrides profile wet cap while SPACE sets the amount |
| Advanced Tail | seconds, 0.08–8; optional | Overrides natural decay; Gated termination clamps to 4 s |

## Initial twelve source/engine profiles

Values are `(HP Hz / LP Hz / Send / Decay s / Pre-delay ms / Damping / Width / Gate s / Density / Wet)`.

| Target | Room | Spring | Gated |
|---|---|---|---|
| Shells | `85 / 15000 / .65 / .80 / 9 / .60 / .75 / .35 / .58 / .72` | `100 / 14000 / .58 / 1.00 / 12 / .42 / .85 / .38 / .72 / .68` | `85 / 16000 / .78 / .58 / 4 / .52 / .90 / .30 / .84 / .80` |
| Snare | `70 / 17000 / .76 / 1.35 / 16 / .42 / 1.05 / .32 / .72 / .80` | `90 / 16000 / .70 / 1.65 / 22 / .30 / 1.12 / .42 / .80 / .74` | `80 / 17000 / .92 / .85 / 6 / .44 / 1.16 / .28 / .92 / .96` |
| Room | `90 / 13000 / .52 / 1.50 / 4 / .62 / .90 / .40 / .82 / .62` | `130 / 12000 / .45 / 1.15 / 9 / .52 / .82 / .45 / .68 / .55` | `110 / 11000 / .45 / .75 / 1 / .70 / .80 / .22 / .72 / .48` |
| Overheads | `170 / 9000 / .38 / .72 / 5 / .82 / .72 / .40 / .65 / .48` | `200 / 8500 / .32 / .82 / 8 / .72 / .70 / .45 / .62 / .38` | `180 / 9000 / .38 / .48 / 0 / .85 / .66 / .16 / .68 / .43` |

The source-controlled table is in `Source/TargetProfiles.cpp`; `Tests/TargetProfilesTests.cpp` verifies distinctness, ranges, finite output, and mono/stereo processing. Twelve SPACE curves are in `Source/SpaceMacro.cpp`; `Tests/SpaceMacroTests.cpp` verifies endpoints, input clamps, smoothing, and exact dry output at zero.

Factory presets are defined in `Source/PluginState.cpp`: Tight Shells, Big Snare, Explosive Snare, Small Studio, Large Room, Wide Overheads, Metallic Spring, and Vintage Gated. Portable state format version 1 serializes the selected target/engine, SPACE, sync/division, free decay, advanced override enable flags, and values. Host IDs and actual plugin state callbacks are still pending.

Planned host controls include Target, Character, SPACE, sync mode, BPM/tap tempo, musical division, and optional Pre-delay, Tone, Width, Mix, and Tail controls. Host IDs, automation, smoothing, and state persistence will be specified when the plugin wrapper is implemented.
