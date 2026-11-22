# DSP Design

No DSP is implemented yet. This document will record algorithms and measured behavior as they are built and reviewed.

## Planned engines

- **Room:** natural early reflections and controlled diffuse decay.
- **Spring:** resonant, characterful delay/feedback network with bounded feedback.
- **Gated:** reverb excitation plus a separately controlled termination envelope for accurate musical tail cutoff.

## Source profiles and SPACE

The four user-selected sources (Shells, Snare, Room, Overheads) will each tune all three engines. SPACE (0–100%) will map to multiple smoothed DSP parameters for each of the twelve combinations; 0% must be fully dry. Do not treat RT60 as the audible end of a natural tail.

## Safety and verification

Document stability bounds, denormal handling, transient and stereo behavior, timing accuracy, and tests alongside each implementation. No audio quality claim has been verified yet.
