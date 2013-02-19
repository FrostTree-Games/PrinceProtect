Prince Protect
(Published by FrostTree Games)
=============

INFORMATION:

-----

A small game written by Daniel Savage over the summer in C and SDL. The source code is released under the MIT License. Included assets (this means anything not README, CODE, or SDL-related), except for the FrostTree Games logo, are available for re-use under the Creative Commons Attribution-NonCommercial 3.0 Unported license. You can re-compile and re-sell my game if you want, but you'll need to provide your own art.

http://creativecommons.org/licenses/by-nc/3.0/

Copyright (C) 2012,2013 Daniel Savage

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

-----

COMPILING:

To compile, simply run GNU Make in the folder:
$ mingw32-make

To remove all .exe and .o files:
$ mingw32-make clean

(the semicolon is your friend!)

Prince Protect is compiled with the MinGW port of gcc along with the following libraries:
- SDL version 1.2
- SDL_image
- SDL_mixer
- SDL_ttf

DLLs used for running the program originally are included in the repo. Using a newer one years from now (2013) may prove difficult at times.

-----
