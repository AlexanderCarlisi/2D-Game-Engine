#ifndef ENGINE_OPTIONS_H
#define ENGINE_OPTIONS_H

#define EO_PROJECT_MAIN         "MyGame.h"
#define EO_PROJECT_MAIN_START   game_init()
#define EO_PROJECT_MAIN_LOOP    game_loop()
#define EO_PROJECT_MAIN_FREE    game_free()

/// Amount of Windows you can have.
/// Allocated statically as WindowConfig* in platform
#define EO_WINDOWS_AMOUNT       1

/// Amount of Game Objects to statically allocate in world
#define EO_GAME_OBJECT_BUFFER   50

/// The Color to render undrawn over pixels as.
/// The framebuffer is cleared with this color before calling rendering over it.
#define EO_RENDER_CLEAR         rgba(255, 255, 255, 255)

/// Path to log file
#define EO_LOG_FILE             "log.txt"

#endif // ENGINE_OPTIONS_H
