# OCR Screen Capture Setup

`ocr.py` captures a screen region and prints recognized text. It uses `sct.monitors[0]` when run without arguments, which captures the virtual desktop across all monitors. To capture a specific region, pass four integers: `x y width height`.

`rapidocr-onnxruntime` 1.4.4 declares support for Python versions below 3.13, so use Python 3.12. It can be installed alongside Python 3.14.

## CPU setup

Run these commands in PowerShell from the `keymacro` folder. Skip the first command if Python 3.12 is already installed.

```powershell
winget install --id Python.Python.3.12 --exact
py -3.12 -m venv .venv
.\.venv\Scripts\python.exe -m pip install mss pillow rapidocr-onnxruntime
.\.venv\Scripts\python.exe .\ocr.py
```

The `RapidOCR()` default configuration uses the CPU. The virtual-desktop capture can be tested independently of OCR with:

```powershell
.\.venv\Scripts\python.exe -c "import mss; s=mss.MSS(); r=s.monitors[0]; shot=s.grab(r); print('Region:', r); print('Captured size:', shot.size); s.close()"
```

## Optional NVIDIA GPU setup

The GPU runtime must be compatible with the installed CUDA and cuDNN libraries. In the virtual environment, replace the CPU ONNX Runtime package and check that CUDA is available:

```powershell
.\.venv\Scripts\python.exe -m pip uninstall -y onnxruntime
.\.venv\Scripts\python.exe -m pip install onnxruntime-gpu
.\.venv\Scripts\python.exe -c "import onnxruntime as ort; print(ort.get_available_providers())"
```

To request CUDA for all OCR stages, construct the engine with:

```python
ocr = RapidOCR(det_use_cuda=True, cls_use_cuda=True, rec_use_cuda=True)
results, _ = ocr(img)
```

`CUDAExecutionProvider` must be available, and the CUDA flags must be enabled; installing the GPU runtime alone does not make `RapidOCR()` use the GPU by default.
