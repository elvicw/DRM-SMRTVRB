#!/usr/bin/env python3
"""Render each generated synthetic fixture through each reverb engine."""

import argparse
import subprocess
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--renderer", default=str(ROOT / "build" / "drmsmrtvrb_render"),
                        help="path to drmsmrtvrb_render")
    parser.add_argument("--directory", default=str(ROOT / "recordings"),
                        help="directory containing synthetic-*.wav fixtures")
    parser.add_argument("--tail", type=float, default=4.0, help="seconds of appended wet tail")
    args = parser.parse_args()

    directory = Path(args.directory)
    fixtures = sorted(directory.glob("synthetic-*.wav"))
    if not fixtures:
        parser.error(f"No synthetic-*.wav fixtures found in {directory}; run GenerateTestRecordings.py first")
    output_dir = directory / "rendered"
    output_dir.mkdir(parents=True, exist_ok=True)
    settings = {
        "room": ("1.2", "8", "0.45", "1", "0.35"),
        "spring": ("0.9", "4", "0.30", "1.2", "0.35"),
        "gated": ("0.8", "8", "0.55", "1", "0.24"),
    }
    for fixture in fixtures:
        for engine, (decay, predelay, damping, width, gate) in settings.items():
            output = output_dir / f"{fixture.stem}-{engine}.wav"
            command = [args.renderer, "--engine", engine, "--input", str(fixture), "--output", str(output),
                       "--decay", decay, "--predelay", predelay, "--damping", damping,
                       "--width", width, "--gate", gate, "--tail", str(args.tail)]
            print(f"Rendering {fixture.name} -> {output.name}", flush=True)
            subprocess.run(command, check=True)
    print(f"Rendered {len(fixtures) * len(settings)} synthetic combinations to {output_dir}")


if __name__ == "__main__":
    main()
