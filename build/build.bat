@echo off
pushd ..\bin
cl -EHsc /MD -O2 -Zi^
    /openmp^
    /I "..\deps\sdl\include"^
    /I "..\deps\glew\include"^
    ..\code\sdl_cg1.cpp^
    /link user32.lib kernel32.lib opengl32.lib glu32.lib^
    /LIBPATH:..\deps\sdl\libs\ SDL2.lib SDL2main.lib^
    /LIBPATH:..\deps\glew\libs\ glew32.lib^
	/SUBSYSTEM:WINDOWS	
REM	/SUBSYSTEM:CONSOLE
REM IF EXIST *.ilk del *.ilk
REM IF EXIST *.obj del *.obj
popd
