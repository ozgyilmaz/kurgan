#!/usr/bin/env python3

import os
import subprocess
from pathlib import Path

AREA_DIR = Path("../archived_area")
SCRIPT_PATH = Path(__file__).resolve().parent / "area-to-json.py"

def is_valid_area_file(filename):
    return (
        filename.endswith(".are") or
        filename == "music.txt"
    )

def main():
    for file in sorted(AREA_DIR.iterdir()):
        if file.is_file() and is_valid_area_file(file.name):
            print(f"Converting: {file}")
            subprocess.run(["/usr/bin/python3", str(SCRIPT_PATH), str(file)])

if __name__ == "__main__":
    main()