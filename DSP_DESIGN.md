# DSP Design

The first core engine pass is implemented in `Source/ReverbEngines.*`. It is a development baseline, not yet tuned against real drum recordings or integrated into a plugin.

## Planned engines

- **Room:** five weighted early-reflection taps feed a four-line orthogonal feedback tank and two allpass diffusion stages.
- **Spring:** four differently timed resonant lines feed a serial dispersive allpass path for a more metallic response.
- **Gated:** short-delay dense feedback tank followed by a separately timed gate envelope; gate duration is not treated as RT60.

The engines expose decay, pre-delay, damping, width, and gate duration. These are raw engine controls; target-specific profiles and SPACE mapping are later milestones.

## Source profiles and SPACE

The four user-selected sources (Shells, Snare, Room, Overheads) will each tune all three engines. SPACE (0–100%) will map to multiple smoothed DSP parameters for each of the twelve combinations; 0% must be fully dry. Do not treat RT60 as the audible end of a natural tail.

## Safety and verification

Delay buffers are prepared outside processing. Feedback is bounded, parameter inputs are clamped, and non-finite output samples are zeroed. The current automated impulse tests check finite/bounded response, nonzero output, engine distinction, mono routing, and gated tail reduction. Real-time behavior, transient preservation, and audio quality still need listening tests with drum recordings and host automation.
