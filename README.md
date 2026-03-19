# 2D Game Engine

## Abstract
This project is a hobby 2D Game Engine written in C. The purpose of this project is to use as little libraries as reasonable, to learn as much as possible.

## Current Progress
Currently there is a simple Engine Framework implemented that includes: GameObjects, Worlds, Shapes, Renderers, Windows, Inputs, and more.

There is an example game in 'MyGame.c', that is used to refine the framework, and as a basic test for the Engine and how it's intended to be implemented for a game.

Windows and Linux (X11) build and run with support for Rendering, and Keyboard Inputs.

Rendering is currently only implemented using a Scanline algorithm utilizing the Nonzero Rule for Convex and Concave shapes. Currently supports different Framerates, resolutions, window sizes, and more.

Input handling is very simple for both Linux and Windows.

Logging is honestly pretty shweet right now, but I do wanna clean it up a bit. Also nothing gets logged if a error occurs which defeats half of its purpose.

Multiple Window support is a thing, just completely untested because I'm too scared, and it's like not a big priority I just thought it was a cool idea.

Windows have references to Worlds, so you can point to the same World object on Multiple Windows or have different ones, so it's actually gonna be pretty cool if it works.

## Plans
- Audio
- Physics, Collisions
- Textures
- Rendering with the GPU (OpenGL, Vulkan, etc), I really want to get as deep into this as I can, and was a big reason I started this project in the first place, so expect a lot of code that never actually gets used, but was just fun to learn and implement.
- And probably more

## How to Build and Run
### Windows
From root dir of project run:

```bash
gcc src/*.c example/src/*.c -Iinclude -Iexample/include -o 2DGameEngine -lgdi32
```

### Linux
Be on (or at least support) X11, I have literally no clue what will happen if you run this on Wayland.

From root dir of project run:

```bash
gcc src/*.c example/src/*.c -Iinclude -Iexample/include -o 2DGameEngine -lxcb -lxcb-shm -lm
```

If you're having dependancy issues on Linux, google the dependancies GCC is saying you're missing ¯\_(ツ)_/¯

Run using `./2DGameEngine 1`

The extra parameter can be either 1 or 0, it dictates how logging is handled, don't worry about it.
The program will yell at you if you forget the 1 or 0, the insult is not directed towards you but myself for debugging for like 10 minutes trying to figure out why nothing was happening.
