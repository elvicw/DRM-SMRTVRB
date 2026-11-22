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
    parser.add_argument("--space", type=float, default=65.0, help="SPACE macro position, 0..100")
    args = parser.parse_args()

    directory = Path(args.directory)
    fixtures = sorted(directory.glob("synthetic-*.wav"))
    if not fixtures:
        parser.error(f"No synthetic-*.wav fixtures found in {directory}; run GenerateTestRecordings.py first")
    output_dir = directory / "rendered"
    output_dir.mkdir(parents=True, exist_ok=True)
    targets = {
        "synthetic-kick-shells": "shells",
        "synthetic-snare": "snare",
        "synthetic-toms": "shells",
        "synthetic-room-mic": "room",
        "synthetic-overheads": "overheads",
    }
    engines = ("room", "spring", "gated")
    for fixture in fixtures:
        target = targets.get(fixture.stem)
        if target is None:
            print(f"Skipping unknown fixture: {fixture.name}")
            continue
        for engine in engines:
            output = output_dir / f"{fixture.stem}-{engine}.wav"
            command = [args.renderer, "--engine", engine, "--input", str(fixture), "--output", str(output),
                       "--target", target, "--space", str(args.space), "--tail", str(args.tail)]
            print(f"Rendering {fixture.name} -> {output.name}", flush=True)
            subprocess.run(command, check=True)
    print(f"Rendered {sum(f.stem in targets for f in fixtures) * len(engines)} synthetic profile combinations to {output_dir}")


if __name__ == "__main__":
    main()
