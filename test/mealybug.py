#!/usr/bin/env python3
"""Run the Mealybug Tearoom Tests and diff each screen against the reference.

  make test-mealybug                 # all DMG tests
  python3 test/mealybug.py -f scx    # only tests whose name contains "scx"
  python3 test/mealybug.py -m cgb-c  # score against CPU CGB C references

bin/mealybug runs one ROM to its LD B,B breakpoint and dumps a PGM; this script
compares that to expected/<model>/<test>.png. Since these tests are all about
what the ppu does mid-scanline, a failure reports which scanlines went wrong,
and writes a side-by-side expected/actual/diff PNG under test/mealybug/results/.

Every ROM has the CGB flag clear, so the emulator always runs them as a DMG and
"dmg" is the only model it can currently be scored against. The CGB references
are what a CGB produces running these DMG ROMs in compatibility mode, so they
are only meaningful once that mode is emulated separately.
"""

import argparse
import os
import subprocess
import sys

from PIL import Image

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
BASE = os.path.join(ROOT, "test", "mealybug")

MODELS = {
    "dmg": "DMG-blob",
    "dmg-b": "DMG-CPU B",
    "cgb-c": "CPU CGB C",
    "cgb-d": "CPU CGB D",
}

GREEN, RED, YELLOW, DIM, OFF = "\033[32m", "\033[31m", "\033[33m", "\033[2m", "\033[0m"


def color(s, c):
    return f"{c}{s}{OFF}" if sys.stdout.isatty() else s


def line_ranges(lines):
    """[1,2,3,7] -> "1-3, 7" """
    out, start = [], None
    for i, y in enumerate(lines):
        if start is None:
            start = y
        if i + 1 == len(lines) or lines[i + 1] != y + 1:
            out.append(str(start) if start == y else f"{start}-{y}")
            start = None
    return ", ".join(out)


def diff_image(expected, actual, path):
    """expected | actual | differing pixels in red, side by side and 2x scale."""
    w, h = expected.size
    canvas = Image.new("RGB", (w * 3 + 8, h), (0, 0, 255))
    canvas.paste(expected.convert("RGB"), (0, 0))
    canvas.paste(actual.convert("RGB"), (w + 4, 0))

    mask = Image.new("RGB", (w, h))
    ep, ap, mp = expected.load(), actual.load(), mask.load()
    for y in range(h):
        for x in range(w):
            v = ep[x, y]
            mp[x, y] = (255, 0, 0) if v != ap[x, y] else (v // 3, v // 3, v // 3)
    canvas.paste(mask, (w * 2 + 8, 0))
    canvas.resize((canvas.width * 2, canvas.height * 2), Image.NEAREST).save(path)


def run_one(runner, rom, expected_png, outdir, timeout_frames, keep_pass):
    name = os.path.basename(expected_png)[:-4]
    pgm = os.path.join(outdir, name + ".pgm")

    env = dict(os.environ, SDL_AUDIODRIVER="dummy",
               MEALYBUG_TIMEOUT=str(timeout_frames))
    proc = subprocess.run([runner, rom, pgm], env=env, capture_output=True, text=True)
    note = proc.stderr.strip().splitlines()
    if proc.returncode == 2 or not os.path.exists(pgm):
        return name, "ERROR", 0, [], note

    expected = Image.open(expected_png).convert("L")
    actual = Image.open(pgm).convert("L")
    if expected.size != actual.size:
        return name, "ERROR", 0, [], ["size mismatch"]

    ep, ap = expected.load(), actual.load()
    bad_lines, bad_pixels = [], 0
    for y in range(expected.height):
        n = sum(1 for x in range(expected.width) if ep[x, y] != ap[x, y])
        if n:
            bad_lines.append(y)
            bad_pixels += n

    if bad_pixels:
        diff_image(expected, actual, os.path.join(outdir, name + ".diff.png"))
    elif not keep_pass:
        os.remove(pgm)

    status = "PASS" if bad_pixels == 0 else "FAIL"
    # A timeout (rc 1) still produces a screen, but the run never reached the
    # breakpoint, so say so even if the picture happens to match.
    if proc.returncode == 1:
        status = "TIMEOUT" if bad_pixels else "PASS?"
    return name, status, bad_pixels, bad_lines, note


def main():
    p = argparse.ArgumentParser()
    p.add_argument("-m", "--model", default="dmg", choices=sorted(MODELS),
                   help="which reference screenshots to score against")
    p.add_argument("-f", "--filter", default="", help="substring of the test name")
    p.add_argument("-t", "--timeout", type=int, default=120,
                   help="frames to run before giving up on the breakpoint")
    p.add_argument("--runner", default=os.path.join(ROOT, "bin", "mealybug"))
    p.add_argument("--keep-pgm", action="store_true",
                   help="keep the dumped screen of passing tests too")
    args = p.parse_args()

    roms = os.path.join(BASE, "roms")
    expected_dir = os.path.join(BASE, "upstream", "expected", MODELS[args.model])
    if not os.path.isdir(roms) or not os.path.isdir(expected_dir):
        sys.exit("test data missing - run: sh test/fetch_mealybug.sh")
    if not os.path.exists(args.runner):
        sys.exit(f"{args.runner} missing - run: make bin/mealybug")

    outdir = os.path.join(BASE, "results", args.model)
    os.makedirs(outdir, exist_ok=True)

    results = []
    for png in sorted(os.listdir(expected_dir)):
        if not png.endswith(".png") or args.filter not in png:
            continue
        rom = os.path.join(roms, png[:-4] + ".gb")
        if not os.path.exists(rom):
            continue
        r = run_one(args.runner, rom, os.path.join(expected_dir, png), outdir,
                    args.timeout, args.keep_pgm)
        results.append(r)

        name, status, bad_pixels, bad_lines, note = r
        tint = {"PASS": GREEN, "PASS?": YELLOW}.get(status, RED)
        detail = ""
        if bad_pixels:
            detail = (f"{bad_pixels:5d} px  {len(bad_lines):3d} lines"
                      f"  [{line_ranges(bad_lines)}]")
        print(f"{name:36s} {color(status.ljust(7), tint)} {detail}")
        for n in note:
            print(color(f"    {n}", DIM))

    passed = sum(1 for r in results if r[1].startswith("PASS"))
    print(f"\n{passed}/{len(results)} passed"
          + (f"  (diffs in {os.path.relpath(outdir, ROOT)})" if passed < len(results) else ""))
    return 0 if passed == len(results) else 1


if __name__ == "__main__":
    sys.exit(main())
