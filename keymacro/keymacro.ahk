/*
===========================================================================
 How to Install AutoHotkey v2 on Windows 11:
 1. Option A (Terminal): Open PowerShell or Terminal and run:
      winget install -e --id AutoHotkey.AutoHotkey
 2. Option B (Installer): Download AutoHotkey v2.0 from:
      https://autohotkey.com
 3. Place keymacro.ahk and keymacro.ini in the same folder, then
    double-click keymacro.ahk to run (green 'H' icon appears in tray).
===========================================================================
*/
#Requires AutoHotkey v2.0
#SingleInstance Force

ini := RegExReplace(A_ScriptFullPath, "\.ahk$", ".ini")

; Read and register the emergency stop hotkey
stopKey := IniRead(ini, "Settings", "stopHotkey", "F12")
if (stopKey != "")
    Hotkey(stopKey, (*) => Reload())

; Read all key=count lines from the [Hotkeys] section and bind them
hotkeySection := IniRead(ini, "Hotkeys")
for line in StrSplit(hotkeySection, "`n", "`r") {
    parts := StrSplit(line, "=", , 2)
    if (parts.Length = 2 && Trim(parts[1]) != "") {
        BindTrigger(Trim(parts[1]), Integer(Trim(parts[2])))
    }
}

BindTrigger(triggerKey, repeatCount) {
    Hotkey(triggerKey, (*) => RunSequence(triggerKey, repeatCount))
}

RunSequence(triggerKey, repeatCount) {
    KeyWait(triggerKey)
    target := "ahk_exe " IniRead(ini, "Settings", "targetExe")
    minDelay := Integer(IniRead(ini, "Settings", "minDelay"))
    maxDelay := Integer(IniRead(ini, "Settings", "maxDelay"))
    keyList := StrSplit(IniRead(ini, "Settings", "keys"), ",")

    if WinExist(target) {
        WinActivate(target)
        WinWaitActive(target)
        
        Loop repeatCount {
            for k in keyList {
                Send(Trim(k))
                Sleep(Random(minDelay, maxDelay))
            }
        }
    }
}