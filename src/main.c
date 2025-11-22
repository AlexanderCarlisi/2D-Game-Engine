// WIN: gcc src/*.c -Iinclude -o 2DGameEngine -lgdi32
// LINUX: gcc src/*.c -Iinclude -o 2DGameEngine -lxcb -lxcb-shm -lm

// TODO: ADDITION: GameObject Textures
// TODO: ADDITION: World Physics, Collisions, CollisionBoxes
// TODO: ADDITION/REFAC: Error Logging to File.

#include "engine_options.h"
#include EO_PROJECT_MAIN

int main() {
    EO_PROJECT_MAIN_START;
    return 0;
}
