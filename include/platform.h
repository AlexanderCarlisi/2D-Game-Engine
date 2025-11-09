#ifndef PLATFORM_H
#define PLATFORM_H

// X11: 
// Read about x11: https://www.x.org/archive/X11R7.5/doc/libX11/libX11.html
// Somewhat read a somewhat documented lib XCB: https://xcb.freedesktop.org/

#include <stdint.h>
#include <stdbool.h>
#include <xcb/xproto.h>
#include "world_handler.h"

#define PLATFORM_MAX_WINDOW_NAME 256    // Should be max Window Name length for Windows and Unix, needs to be tested.

#ifdef _WIN32
#define WINDOWINFO W32Window

#else
#define WINDOWINFO X11Window

#endif


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
    uint32_t* framebuffer;
    struct WorldHandler* world_handler;
    struct Aspect window_aspect;
    struct Aspect render_aspect;
    float frames_per_second;
    float previous_time;
    float accumulator;
} WindowConfig;

DECLARE_VECTOR(WindowConfigVector, vector_window_config, struct WindowConfig);

/// @brief Startup the platform.
/// @return Success.
void platform_initialize();

/// @brief Handles the creation and initialization of a new Window for the respective platform.
/// TODO: doxy
/// @see X11Window
/// @see TODO: Window's Window
/// @note config param is deep copied into return value.
struct WINDOWINFO* platform_new_window(char* windowName, struct Aspect windowSize, struct Aspect resolution, float fps);

/// @brief Flushes all changes of the Window Config to the platform.
/// @param config WindowConfig with changes you wish to see implemented.
/// @return Success.
bool platform_update_window(struct WINDOWINFO* window);

/// @brief Update the name of the Window, flushes change.
/// @return Success.
/// @see platform_new_window for changing multiple fields at once.
bool platform_set_window_name(struct WINDOWINFO* window, const char* name);

/// @brief Update the window size, flushes change.
/// @return Success.
/// @see platform_new_window for changing multiple fields at once.
bool platform_set_window_size(struct WINDOWINFO* window, struct Aspect size);

/// @brief Update the window resolution, flushes change.
/// @return Success.
/// @see platform_new_window for changing multiple fields at once.
bool platform_set_window_resolution(struct WINDOWINFO* window, struct Aspect res);

/// @brief Renders the World assigned to the WindowConfig.
/// @param config The WindowConfig to render.
/// @return False if an error occurs.
bool platform_render(struct WINDOWINFO* window, float alpha);

/// @brief Performs an iteration platform side.
/// Checks for client side inputs and requests.
/// @param config The window to perform checks on.
/// @return False if an error occurs.
bool platform_iterate(struct WINDOWINFO* window);

/// @brief Deallocates all dynamic memory of the platform
/// @see engine_close
void platform_free();

// TODO: WINDOWS
#ifdef _WIN32

#include <windows.h>
#include <wingdi.h>
#include <windows.h>
#include <windef.h>

typedef struct W32Window {
    struct WindowConfig config;
    HWND hwnd;
    HDC hdc;
    HBITMAP bitmap;
    BITMAPINFO bitmapInfo;
    HBITMAP oldBitMap;
} W32Window;

#endif

#ifdef __linux__
#include <xcb/xcb.h>
#include <xcb/shm.h>
#include <xcb/xcb_image.h>

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
    
    xcb_gcontext_t gc;
    xcb_image_t* image;
    xcb_shm_seg_t shm_seg;
    int shm_id;
} X11Window;

#endif // __linux__
#endif // PLATFORM_H
