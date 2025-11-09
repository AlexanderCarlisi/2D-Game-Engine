#ifndef ENGINE_OPTIONS_H
#define ENGINE_OPTIONS_H

#define EO_PROJECT_MAIN         "../test/MyGame.c"
#define EO_PROJECT_MAIN_START   init()
#define EO_PROJECT_MAIN_LOOP    loop()

/// Amount of Windows you can have.
/// Allocated statically as WindowConfig* in platform
#define EO_WINDOWS_AMOUNT       1

/// Amount of Game Objects to statically allocate in world
#define EO_GAME_OBJECT_BUFFER   100

#endif // ENGINE_OPTIONS_H
