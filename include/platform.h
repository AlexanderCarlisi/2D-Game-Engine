#ifndef PLATFORM_H
#define PLATFORM_H

#include <stdint.h>
#include <stdbool.h>
#include "world.h"

#define PLATFORM_MAX_WINDOW_NAME 256    // Should be max Window Name length for Windows and Unix, needs to be tested.

/// @struct Aspect
/// A width and height representing pixels.
/// Layout [uint16, uint16]
/// Size 4(8), 4
typedef struct Aspect {
    uint16_t width;
    uint16_t height;
} Aspect;


/// @struct WindowConfig
/// Contains information consering that of a Window.
/// This information is used the same across platforms.
///
/// @var world A pointer to the desired World to render. The purpose behind it
///     being a pointer is so that you can have multiple windows render the same
///     world. This does add additional complexity, TODO: Look into maybe making
///     it easier to work with, but also test it out, see how bad it really is.
typedef struct WindowConfig {
    char window_name[PLATFORM_MAX_WINDOW_NAME];
    struct Aspect window_aspect;
    struct Aspect render_aspect;
    float frames_per_second;
    struct World* world;
} WindowConfig;


/// @brief Startup the platform.
/// @return Success.
bool platform_initialize();

/// @brief Handles the creation and initialization of a new Window for the respective platform.
/// @return WindowConfig pointer. Can be type casted to the specific platform's Window struct.
/// @see X11Window TODO:
struct WindowConfig* platform_new_window(struct WindowConfig* config);

/// @brief Flushes all changes of the Window Config to the platform.
/// @param config WindowConfig with changes you wish to see implemented.
/// @return Success.
bool platform_update_window(struct WindowConfig* config);

/// @brief Update the name of the Window, flushes change.
/// @return Success.
/// @see platform_new_window for changing multiple fields at once.
bool platform_set_window_name(struct WindowConfig* config, const char* name);

/// @brief Update the window size, flushes change.
/// @return Success.
/// @see platform_new_window for changing multiple fields at once.
bool platform_set_window_size(struct WindowConfig* config, struct Aspect size);

/// @brief Update the window resolution, flushes change.
/// @return Success.
/// @see platform_new_window for changing multiple fields at once.
bool platform_set_window_resolution(struct WindowConfig* config, struct Aspect res);

/// @brief Performs an iteration platform side.
/// Checks for client side inputs and requests.
/// @param config The window to perform checks on.
/// @return False if an error occurs.
bool platform_iterate(struct WindowConfig* config);

/// @brief Renders the World assigned to the WindowConfig.
/// @param config The WindowConfig to render.
/// @return False if an error occurs.
bool platform_render(struct WindowConfig* config);

// TODO: WINDOWS
#ifdef _WIN32
#endif

#ifdef __linux__
#include <xcb/xcb.h>

/**
 * @struct X11Window
 * The container holding all information relevant to a single X11 Window.
 * Used for Rendering, Inputs, ---- and whatever else gets added. TODO:
 */
typedef struct X11Window {
    struct WindowConfig config;
    xcb_connection_t* connection;
    xcb_screen_t* screen;
    xcb_window_t window;
    xcb_pixmap_t pixmap;
    uint32_t* framebuffer;
} X11Window;

#endif // __linux__
#endif // PLATFORM_H
