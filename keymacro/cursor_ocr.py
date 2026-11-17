#!/usr/init/env python3
import sys
import time
import mss
from PIL import Image
from rapidocr_onnxruntime import RapidOCR
from pynput import mouse

# Initialize OCR engine once globally
ocr_engine = RapidOCR()

def capture_at_cursor(box_width=300, box_height=80):
    """Captures a region using the current mouse cursor as the top-left corner."""
    controller = mouse.Controller()
    x, y = controller.position
    
    # Cursor position is now the top-left corner
    region = {
        "left": int(x),
        "top": int(y),
        "width": box_width,
        "height": box_height
    }
    
    with mss.MSS() as sct:
        raw = sct.grab(region)
        img = Image.frombytes("RGB", raw.size, raw.bgra, "raw", "BGRX")
        
    return img, region

def run_ocr_on_image(img):
    """Runs RapidOCR on a PIL Image and prints the text."""
    results, _ = ocr_engine(img)
    print("\n--- OCR Results ---")
    if results:
        for _, text, score in results:
            print(f"[{score:.2f}] {text}")
    else:
        print("(No text detected in this region)")
    print("-------------------")

def on_click(x, y, button, pressed):
    """Trigger OCR on middle click using cursor as top-left."""
    if pressed and button == mouse.Button.middle:
        print(f"\n[Triggered by Middle Click at X:{x}, Y:{y}]")
        img, region = capture_at_cursor(box_width=300, box_height=80)
        print(f"Captured region: {region}")
        run_ocr_on_image(img)

def main():
    # CLI Mode (optional manual coordinates)
    if len(sys.argv) >= 5:
        x, y, w, h = map(int, sys.argv[1:5])
        region = {"left": x, "top": y, "width": w, "height": h}
        with mss.MSS() as sct:
            raw = sct.grab(region)
            img = Image.frombytes("RGB", raw.size, raw.bgra, "raw", "BGRX")
        run_ocr_on_image(img)
        return

    print("General OCR Tool Initialized (Top-Left Cursor Mode).")
    print("Options:")
    print(" 1. Press ENTER in this terminal to OCR right from your cursor's position.")
    print(" 2. Click your MIDDLE mouse button to capture from the cursor.")
    print("Press Ctrl+C to exit.\n")

    listener = mouse.Listener(on_click=on_click)
    listener.start()

    try:
        while True:
            input()
            img, region = capture_at_cursor(box_width=300, box_height=80)
            print(f"Captured region: {region}")
            run_ocr_on_image(img)
    except KeyboardInterrupt:
        print("\nExiting...")
        listener.stop()

if __name__ == "__main__":
    main()