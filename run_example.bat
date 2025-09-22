del out.exe
del *.obj
cl "examples\platform\platform_win32.c" "examples\%1" user32.lib gdi32.lib /W3 /Fe:out.exe /Z7 /O2
out.exe
