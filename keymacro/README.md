# KeyMacro for Windows 11

A lightweight, general-purpose key-sequence automation tool for **Windows 11** powered by **AutoHotkey v2**. 

Configure your target application, randomized keystroke delays, key sequences, and custom hotkeys entirely inside a simple `keymacro.ini` file without ever touching the script code.

---

## Features
* **Zero-Code Configuration:** Change target apps, delays, key sequences, and hotkeys directly in `keymacro.ini`.
* **Randomized Intervals:** Sets a random delay (`minDelay` to `maxDelay`) between every keystroke for natural timing.
* **Unlimited Custom Hotkeys:** Bind as many single keys (`F9`, `F10`) or modifier combos (`^!r`) as you want, each with its own repeat count.
* **Instant Stop & Reload:** Press your configured emergency stop key (`F12` by default) to halt any active loop and reload updated settings immediately.

---

## Prerequisites & Installation (Windows 11)

This tool requires **AutoHotkey v2.0** or newer.

### Option 1: Install via Windows Terminal (Recommended)
1. Open **Terminal** or **PowerShell**.
2. Run the following command:
   ```powershell
   winget install -e --id AutoHotkey.AutoHotkey
   ```

### Option 2: Install via Official Website
1. Visit the [AutoHotkey Official Website](https://autohotkey.com).
2. Download and run the **AutoHotkey v2.0** installer.

---

## Getting Started

1. **Download the files:** Place `keymacro.ahk` and `keymacro.ini` in the **same folder**.
2. **Edit your configuration:** Open `keymacro.ini` in Notepad (or any text editor) and customize your settings.
3. **Run the script:** Double-click `keymacro.ahk`. A green **H** icon will appear in your Windows 11 system tray (bottom-right near the clock) to confirm it is running.
4. **Trigger a macro:** Press any hotkey defined under `[Hotkeys]` (e.g., `F9`, `F10`, or `F11`).

---

## Configuration (`keymacro.ini`)

Open `keymacro.ini` to customize how the tool behaves. Use a semicolon (`;`) on its own line to write comments.

```ini
[Settings]
; Target application executable name
targetExe=chrome.exe

; Random delay range in milliseconds between keystrokes
minDelay=1000
maxDelay=1500

; Emergency stop and reload hotkey
stopHotkey=F12

; Comma-separated list of keys to send per cycle
keys=A,R,B,B,{Escape}

[Hotkeys]
; Format: Hotkey=RepeatCount
F9=1
F10=5
F11=20
```

### Configuration Reference

| Setting | Description |
| :--- | :--- |
| **`targetExe`** | The exact `.exe` process name of the window you want to focus and send keys to (e.g., `chrome.exe`, `notepad.exe`, `msedge.exe`). |
| **`minDelay` / `maxDelay`** | Minimum and maximum wait time (in milliseconds) randomly chosen after each keystroke (`1000` = 1 second). |
| **`stopHotkey`** | Immediately cancels a running loop and reloads `keymacro.ini`. |
| **`keys`** | Comma-separated sequence of keys. Wrap special keys in curly braces (e.g., `{Escape}`, `{Enter}`, `{Tab}`, `{Space}`). Note: uppercase letters like `A` send `Shift+a`; use lowercase `a` for standard unshifted keystrokes. |
| **`[Hotkeys]`** | Add any number of `Key=Count` pairs. Supports modifier symbols: `^` (Ctrl), `!` (Alt), `+` (Shift), and `#` (Win). Example: `^!r=50` triggers 50 repeats when pressing `Ctrl + Alt + R`. |

---

## Finding `targetExe` Using AutoHotkey Window Spy

1. Right-click the green **H** AutoHotkey icon in your Windows 11 system tray (bottom-right) and select **Window Spy** (or search **Window Spy** in the Windows Start menu).
2. Move your mouse cursor over the target window you want to automate (Window Spy updates automatically as you hover—no clicking needed).
3. Look at the top box (**Window Title, Class and Process**) and find the line starting with `ahk_exe` (e.g., `ahk_exe chrome.exe`).
4. Copy the `.exe` filename (`chrome.exe`) into `keymacro.ini` as your `targetExe` value.

---

## Tips & Troubleshooting

* **Applying Changes:** 
  * Changes to `targetExe`, `minDelay`, `maxDelay`, or `keys` take effect **immediately** after saving `keymacro.ini`.
  * Changes to `[Hotkeys]` or `stopHotkey` require pressing `F12` (or right-clicking the green **H** tray icon and choosing **Reload Script**).
* **Target App Running as Administrator:** If your target program runs with Administrator privileges, right-click `keymacro.ahk` and select **Run as administrator**, or Windows 11 will block simulated keystrokes.
* **Run Automatically at Startup:** Press `Win + R`, type `shell:startup`, and place a shortcut to `keymacro.ahk` inside the folder that opens.
