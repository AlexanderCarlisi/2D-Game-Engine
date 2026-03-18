# 2D Game Engine

## Abstract
This project is a hobby 2D Game Engine written in C. 
The purpose of this project is to use as little libraries as reasonable, to learn as much as possible.

## Current Progress
Currently there is a simple Engine Framework implemented that includes: GameObjects, Worlds, Shapes, Renderers, Windows, Inputs, and more.
There is an example game in 'MyGame.c', that is used to refine the framework, and as a basic test for the Engine and how it's intended to be implemented for a game.

Windows was implemented originally using 'Windows.h', until I refactored the code for Linux and scope crept a little bit too much, and Windows was just never updated from the refactor.
So the core functionality for Windows exists, it just needs to be rewritten a bit.

Linux (X11) should be working (?), it works on my laptop at least. I genuently could not tell you why I am using XCB, but do know that it wasn't my first choice, and am totally willing to reimplement everything in something else, I knew the reason 4 months ago, I don't remember anymore.
Linux currently has working Windows, Inputs, and Rendering.

Rendering is currently only implemented using a Scanline algorithm utilizing the Nonzero Rule for Convex and Concave shapes. Currently supports different Framerates, resolutions, window sizes, and more.
Input handling is very rudimentry and only implemented for Linux.
Logging also had a crude implementation, but it honestly looks really shweet right now, so it'll be a bit before it gets refactored.
Multiple Window support is a thing (this is why Windows doesn't work), but it's not tested, because I forgot I did it (literally half of my headache on this project and I still am too scared to see if it works).
Windows have references to Worlds, so you can point to the same World object on Multiple Windows or have different ones, so it's actually gonna be pretty cool if it works.

## Plans
Audio
Refactor the Windows Code
Physics, Collisions
Textures
Actual Example Project that isn't one ugly file and is like an actual game
Rendering with the GPU (OpenGL, Vulkan, etc), I really want to get as deep into this as I can, and was a big reason I started this project in the first place, so expect a lot of code that never actually gets used, but was just fun to learn and implement.
And probably more

# How to Build and Run
(Windows is broken right now)
(If you're on Wayland I have literally no clue what will happen)
I compile using gcc, if you can figure something else out you're more than welcome to do whatever you want.

`gcc src/*.c -Iinclude -o 2DGameEngine -lxcb -lxcb-shm -lm`

I didn't document how I downloaded the dependancies, so just google it because thats what I did :)

Run using `./2DGameEngine 1`
the extra parameter can be either 1 or 0, it dictates how logging is handled, don't worry about it.
