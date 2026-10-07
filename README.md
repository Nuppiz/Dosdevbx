Table of contents:
Borwat.h - header file to convert certain Borland-specific standard function calls to Open Watcom
CGAEGA.ASM - Everett Kaser's CGA and EGA routines from 1987 written in x86 assembly, modified by me for clarity and compatibility
Common.h - standard libraries needed for the various functions, plus some custom headers for basic defines and structs
Defines.h - custom defines and macros, mainly to make hardware calls more understandable
Dosdevbx.exe - 16-bit DOS executable for testing things
Dosdevbx.wpj - Open Watcom 2.0 project file for compiling the test suite
Draw.c/h - drawing functions for various graphics modes
FONT.7UP - custom font to allow for clear text display while in graphics modes
HERCULES.C/H - Ben Bederson's Hercules Graphics Card routines from 1988, modified by me for clarity and compatibility
Keyb.c/h - AT-compatible keyboard driver
Keylist.c/h - support files for the keyboard driver to translate scancodes into human-readable button presses
Loadgfx.c/h - functions to load graphics from a file to memory
Main.c - main loop for testing things
Mouse.c/h - basic DOS mouse functions
Structs.h - some basic structs needed to run the test suite
Text.c/h - text drawing functions while in graphics mode
Timer.c/h - Intel 8253-based timer reprogramming functions
Video.c/h - Various functions related to setting up and checking the video display mode
