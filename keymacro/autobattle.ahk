#Requires AutoHotkey v2.0

; Press Esc to trigger the sequence ($ prevents self-triggering on {Escape})
F9:: {
    KeyWait "F9"
    RunKeySequence(1)
}

F10:: {
    KeyWait "F10"
    RunKeySequence(5)
}

F11:: {AR
    KeyWait "F11"
    RunKeySequence(20)
}


; Emergency stop hotkey (reloads the script immediately)
F12::Reload

RunKeySequence(repeatCount) {
    if WinExist("ahk_exe chrome.exe") {
        WinActivate
        WinWaitActive
        
        minDelay := 700  ; Minimum pause in milliseconds
        maxDelay := 900  ; Maximum pause in milliseconds

        Loop repeatCount {
            Send "A"
            Sleep Random(minDelay, maxDelay)

            Send "R"
            Sleep Random(minDelay, maxDelay)

            Send "B"
            Sleep Random(minDelay, maxDelay)

            Send "B"
            Sleep Random(minDelay, maxDelay)

            Send "{Escape}"
            Sleep Random(minDelay, maxDelay)
        }
    }
}
