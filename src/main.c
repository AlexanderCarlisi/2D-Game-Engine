// gcc src/*.c -Iinclude -o 2DGameEngine -lgdi32
// gcc src/*.c -Iinclude -o 2DGameEngine -lxcb -lxcb-shm -lm

// TODO: REFACTOR/ADDITION World Handler
// TODO: ADDITION: GameObject Shapes
// TODO: ADDITION: GameObject Textures
// TODO: ADDITION: World Physics, Collisions, CollisionBoxes
// TODO: ADDITION/REFAC: Error Logging to File.

// TODO: Cleanup GameObject formatting
// TODO: Refactor World handling of GameObjects and Pool
// TODO: Refactor size variables with size_t
// TODO: UNIX impl
// TODO: CLEANUP, go through everything, lots of refactors, theres gonna be tons of uglyness
//
// TODO: Refactor Renderer to not be like a 'object' (maybe)
// TODO: This will probably never happen. but refactor the entire project to allow Multi-Windowness
//          Requires a rewrite of appconfig, the renderers, engine, and platform

// CURRENT GOALS:
//  - Implement Rendering of Shapes / Collision Boxes
//      - Possibly start implementing the rotation on them.
//
//      TODO: NEW STANDARD: TOP LEFT IS (0, 0) for everything,
//      (0, 0) being the bottom right for vertices of shapes no longer.
//      It gets confusing, but if everything is just (0, 0) top left like
//      the framebuffer, life is more simple.
//
//      TODO: Update doxys ^, code will work itself out eventually
//
//  - Implementing Unix support

#include "engine_options.h"
#include EO_PROJECT_MAIN

int main() {
    EO_PROJECT_MAIN_START;
    return 0;
}
