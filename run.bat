del out.exe
cl platform_win32.c "examples\%1" user32.lib gdi32.lib /Fe:out.exe
out.exe
