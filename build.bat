@echo off
call "C:\Program Files (x86)\Microsoft Visual Studio 14.0\VC\bin\amd64\vcvars64.bat"
set CompileFlags=-Zi -nologo -FC -Zi -W4 -wd4505 -wd4201 -wd4100 -wd4244 -Gm- -GR- -EHa- -Oi -GS- -Gs9999999 -D_CRT_SECURE_NO_WARNINGS
set LinkFlags=-link -subsystem:console -stack:0x100000,0x100000 -MAP:example.map

cl %CompileFlags% example.cc user32.lib shell32.lib kernel32.lib wsock32.lib %LinkFlags%

