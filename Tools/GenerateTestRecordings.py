#!/usr/bin/env python3
"""Generate deterministic synthetic drum-like WAV fixtures for DSP testing.

These are procedural test signals, not recordings of real instruments.
Only Python's standard library is required.
"""

import math
import random
import struct
import wave
from pathlib import Path

RATE = 48_000
DURATION = 6.0
FRAMES = int(RATE * DURATION)
OUT = Path(__file__).resolve().parents[1] / "recordings"


def env(t, decay):
    return math.exp(-max(0.0, t) / decay) if t >= 0.0 else 0.0


def sine_hit(t, start, f0, f1, decay, amp=1.0):
    age = t - start
    if age < 0.0:
        return 0.0
    # Exponential pitch sweep is a useful simplified model for drum heads.
    phase = 2.0 * math.pi * (f1 * age + (f0 - f1) * decay * (1.0 - math.exp(-age / decay)))
    return amp * env(age, decay * 1.6) * math.sin(phase)


def make_noise(seed):
    rng = random.Random(seed)
    return [rng.uniform(-1.0, 1.0) for _ in range(FRAMES)]


def make_kick(seed=1):
    noise = make_noise(seed)
    out = [0.0] * FRAMES
    for start, amp, f0, f1, decay in [
        (0.35, 0.95, 125, 43, 0.11), (1.05, 0.82, 112, 48, 0.13),
        (2.10, 1.00, 138, 42, 0.15), (3.55, 0.88, 118, 46, 0.12),
        (4.65, 0.94, 130, 40, 0.16),
    ]:
        click_start = int(start * RATE)
        click_len = int(0.012 * RATE)
        for i in range(click_start, min(FRAMES, click_start + int(0.8 * RATE))):
            t = i / RATE
            body = sine_hit(t, start, f0, f1, decay, amp)
            age = (i - click_start) / RATE
            click = noise[i] * math.exp(-age / 0.0015) * 0.24 if age * RATE < click_len else 0.0
            out[i] += body + click
    return out


def make_snare(seed=2):
    noise = make_noise(seed)
    out = [0.0] * FRAMES
    for start, amp in [(0.55, 1.0), (1.55, 0.82), (2.62, 0.94), (4.08, 0.9), (5.0, 1.0)]:
        onset = int(start * RATE)
        for i in range(onset, min(FRAMES, onset + int(0.55 * RATE))):
            age = (i - onset) / RATE
            wire = noise[i] * (0.8 * math.exp(-age / 0.12) + 0.2 * math.exp(-age / 0.35))
            body = (0.55 * math.sin(2 * math.pi * 185 * age) + 0.24 * math.sin(2 * math.pi * 330 * age)) * math.exp(-age / 0.19)
            crack = noise[i] * math.exp(-age / 0.003) * 0.65
            out[i] += amp * (0.56 * wire + body + crack)
    return out


def make_toms(seed=3):
    noise = make_noise(seed)
    out = [0.0] * FRAMES
    hits = [(0.45, 150, 88), (1.45, 112, 67), (2.45, 91, 54),
            (3.45, 145, 82), (4.05, 106, 61), (4.62, 78, 46)]
    for start, f0, f1 in hits:
        onset = int(start * RATE)
        for i in range(onset, min(FRAMES, onset + int(0.85 * RATE))):
            t = i / RATE
            age = (i - onset) / RATE
            out[i] += sine_hit(t, start, f0, f1, 0.23, 0.9)
            out[i] += noise[i] * math.exp(-age / 0.006) * 0.18
    return out


def make_kit(seed=4):
    kick, snare, toms = make_kick(seed), make_snare(seed + 10), make_toms(seed + 20)
    return [0.72 * kick[i] + 0.65 * snare[i] + 0.62 * toms[i] for i in range(FRAMES)]


def make_room(seed=5):
    dry = make_kit(seed)
    noise = make_noise(seed + 30)
    taps = [(0.0, 0.48), (0.019, 0.34), (0.033, -0.22), (0.051, 0.18),
            (0.074, -0.12), (0.103, 0.08), (0.151, 0.055)]
    out = [0.0] * FRAMES
    for i in range(FRAMES):
        value = dry[i]
        for delay, gain in taps[1:]:
            offset = int(delay * RATE)
            if i >= offset:
                value += dry[i - offset] * gain
        # Quiet diffuse room-like wash after hits, intentionally synthetic.
        out[i] = value + noise[i] * max(0.0, abs(dry[i]) - 0.015) * 0.035
    return out


def make_overheads(seed=6):
    kick, snare = make_kick(seed), make_snare(seed + 10)
    noise = make_noise(seed + 20)
    out = [0.0] * FRAMES
    crashes = [(0.22, 1.0), (2.20, 0.82), (4.25, 0.94)]
    for i in range(FRAMES):
        t = i / RATE
        cymbal = 0.13 * (0.7 * math.sin(2 * math.pi * 3721 * t)
                         + 0.42 * math.sin(2 * math.pi * 5917 * t)
                         + 0.26 * math.sin(2 * math.pi * 8173 * t))
        for start, amp in crashes:
            age = t - start
            if age >= 0.0:
                # High-passed noise plus inharmonic metallic partials.
                high_noise = noise[i] - (noise[i - 1] if i else 0.0)
                cymbal += amp * math.exp(-age / 1.35) * (
                    0.22 * high_noise
                    + 0.09 * math.sin(2 * math.pi * 5033 * age)
                    + 0.055 * math.sin(2 * math.pi * 7349 * age))
        out[i] = cymbal + 0.24 * kick[i] + 0.2 * snare[i]
    return out


def write_fixture(name, mono, pan=0.0):
    path = OUT / name
    OUT.mkdir(parents=True, exist_ok=True)
    frames = bytearray()
    for i, sample in enumerate(mono):
        # Tiny channel-dependent delays and level offsets exercise stereo width.
        left = max(-0.98, min(0.98, sample * (1.0 - 0.12 * pan)))
        right_sample = mono[i - int(0.00023 * RATE)] if i >= int(0.00023 * RATE) else 0.0
        right = max(-0.98, min(0.98, right_sample * (1.0 + 0.12 * pan)))
        frames.extend(struct.pack("<hh", round(left * 32767), round(right * 32767)))
    with wave.open(str(path), "wb") as wav:
        wav.setnchannels(2)
        wav.setsampwidth(2)
        wav.setframerate(RATE)
        wav.writeframes(frames)
    print(f"{path} ({RATE} Hz stereo PCM16, {DURATION:.1f} s; synthetic)")


def main():
    fixtures = [
        ("synthetic-kick-shells.wav", make_kick(), -0.15),
        ("synthetic-snare.wav", make_snare(), 0.1),
        ("synthetic-toms.wav", make_toms(), -0.05),
        ("synthetic-room-mic.wav", make_room(), 0.0),
        ("synthetic-overheads.wav", make_overheads(), 0.2),
    ]
    for name, audio, pan in fixtures:
        write_fixture(name, audio, pan)


if __name__ == "__main__":
    main()
