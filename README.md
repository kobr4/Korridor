# Korridor

## Summary

Oculus Rift VR Maze Demo

It's basically a FPS-like maze exploration demo.

Windows Build : https://github.com/kobr4/Korridor/releases/download/1.0/Korridor.zip
Source code here : https://github.com/kobr4/Korridor/tree/master/Korridor

* Procedurally generated maze: so it looks different each time you run it.
* OpenGL 3.0 (but OpenGL ES 2.0 friendly)
* Should run very fast even on low-end PC
* Xbox360 gamepad support

## Instructions
Just run Korridor.exe with the Oculus Rift DK2 set as the main monitor.

## Compilation

Currently this project only come with a solution file for Visual Studio 2012, compiled for 32bit x86 architecture.

Needed environnement variables
LIBOVR : Path to Oculus Rift SDK (only tested with 0.4.4)
SDLPATH : Path to SDL 2.0 library
SDLTTFPATH : Path to SDL 2.0 TTF
GLEWPATH : Path to GLEW library

In order to compile the "Release" configuration, every dependecies should be linked statically in order to produce a stand-alone executable: 
* GLEW should be compiled as static library and be named glew32s.lib
* SDL 2.0 should be compiled as static library and be named "SDL2s.lib","SDL2_mains.lib", "SDL2_TTFs.lib"
* you additionnaly need the Windows version of UPX (Ultimate Packer for eXecutables). 
UPXPATH variable should point to UPX directory

