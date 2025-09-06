del out.exe
del *.obj
cl platform_win32.c "examples\%1" user32.lib gdi32.lib /W3 /Fe:out.exe
out.exe
