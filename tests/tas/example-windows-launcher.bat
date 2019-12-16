@echo off

%~dp0Injector.exe "D:\Games\SteamLibrary\steamapps\common\Half-Life\hl.exe" %* > nul

:loop
tasklist | find "hl.exe" > nul
if not errorlevel 1 (
    timeout /T 1 > nul
    goto loop
)
