#!/usr/bin/env python3
import sys
import mss
from PIL import Image
from rapidocr_onnxruntime import RapidOCR

def main():
    with mss.MSS() as sct:
        if len(sys.argv) >= 5:
            x, y, w, h = map(int, sys.argv[1:5])
            region = {"left": x, "top": y, "width": w, "height": h}
        else:
            region = sct.monitors[0]

        raw = sct.grab(region)
        img = Image.frombytes("RGB", raw.size, raw.bgra, "raw", "BGRX")

    results, _ = RapidOCR()(img)
    if results:
        for _, text, _ in results:
            print(text)

if __name__ == "__main__":
    main()
