#include "platform.h"
#include "engine.h"
#include <sched.h>
#include <stdio.h>
#include <xcb/xcb.h>
#include <xcb/xproto.h>
#include "time.h"

// TODO: Actually implement the different window configs
DEFINE_VECTOR(WindowConfigVector, vector_window_config, struct WindowConfig);
static WindowConfigVector window_configs;


#ifdef _WIN32

#include <windows.h>

bool platform_initialize() {
    return false;
}

struct WindowConfig* platform_new_window(struct WindowConfig* config) {
    return NULL;
}

bool platform_set_window_name(struct WindowConfig* config, const char* name) {
    return false;
}

bool platform_set_window_size(struct WindowConfig* config, struct Aspect size) {
    return false;
}

bool platform_set_window_resolution(struct WindowConfig* config, struct Aspect res) {
    return false;
}

bool platform_update_window(struct WindowConfig* config) {
    return false;
}

bool platform_iterate(struct WindowConfig* config) {
    return false;
}

bool platform_render(struct WindowConfig* config, float alpha) {
    return false;
}

// static HWND hwnd = NULL;
//
//
// // Windows Message Callback
// LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
//     switch (uMsg) {
//         case WM_CLOSE: DestroyWindow(hwnd); return 0;
//         case WM_DESTROY: PostQuitMessage(0); return 0;
//         case WM_SIZE: {
//             int width = LOWORD(lParam);
//             int height = HIWORD(lParam);
//
//             if (engine_is_running()) {
//                 appconfig_platform_resized_window_px(width, height);
//             }
//
//             return 0;
//         }
//         default: return DefWindowProc(hwnd, uMsg, wParam, lParam);
//     }
// }
//
// void platform_start() {
//     struct ApplicationConfig* config = appconfig_get();
//
//     WNDCLASS wc = {0};
//     wc.lpfnWndProc = WindowProc;
//     wc.hInstance = GetModuleHandle(NULL);
//     wc.lpszClassName = "WindowClass";
//
//     RegisterClass(&wc);
//
//     hwnd = CreateWindowEx(
//         0, "WindowClass", config->window_name,
//         WS_OVERLAPPEDWINDOW,
//         CW_USEDEFAULT, CW_USEDEFAULT,
//         config->window_aspect.width, config->window_aspect.height,
//         NULL, NULL, wc.hInstance, NULL
//     );
//
//     if (!hwnd) {
// printf("HWND is Null... exiting\n");
//         return;
//     }
//
//     ShowWindow(hwnd, SW_SHOW);
//     initialize_time_frequency();
//     engine_start(create_windows_renderer(&hwnd));
//
//     MSG msg = {0};
//     while (1) {
//         while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
//             if (msg.message == WM_QUIT || !engine_is_running()) {
//                 printf("...Closing Properly...\n");
//                 engine_close();
//                 return;
//             }
//             TranslateMessage(&msg);
//             DispatchMessage(&msg);
//         }
//
//         engine_tick();
//         Sleep(1); // TODO: Look into adjusting this
//     }
//
//     engine_close();
// }
//
// void platform_resize_window(struct Aspect* windowSize) {
//     SetWindowPos(hwnd, NULL, 0, 0, windowSize->width, windowSize->height, SWP_NOMOVE | SWP_NOZORDER);
// }
//
// void platform_name_window(const char* name) {
//     SetWindowText(hwnd, name);
// }


#else
// TODO: Linux
#include <stdlib.h>
#include <string.h>

// Atoms are global to the X Server instance
xcb_atom_t ATOM_WM_PROTOCOLS = XCB_NONE;
xcb_atom_t ATOM_WM_DELETE_WINDOW = XCB_NONE;

/// @brief attempt to cast WindowConfig to X11Window.
/// @return X11Window_ptr, or NULL on fail.
struct X11Window* _cast_window(struct WindowConfig* windowConfig) {
    if (windowConfig == NULL) {
        printf("\n>>> _cast_window: NULL Pointer argument <<<\n");
        return NULL;
    }

    return (X11Window*) windowConfig;
}

/// @brief XCB Window Resize Helper function, doesn't flush.
void _resize_window(struct X11Window* window, struct Aspect* windowSize) {
    if (!window->connection) return;
    xcb_configure_window(
        window->connection,
        window->window,
        XCB_CONFIG_WINDOW_WIDTH | XCB_CONFIG_WINDOW_HEIGHT,
        (uint32_t[]){windowSize->width, windowSize->height}
    );
}

/// @brief XCB Window Rename Helper function, doesn't flush.
void _rename_window(struct X11Window* window, const char* name) {
    if (!window->connection) return;

    xcb_change_property(
        window->connection,
        XCB_PROP_MODE_REPLACE,
        window->window,
        XCB_ATOM_WM_NAME, XCB_ATOM_STRING,
        8, strlen(name), name
    );   
}

/// @brief Request an atom from the X11 instance using XCB.
/// @return The requested atom response. 
/// @note Flushes, and Blocking. Deallocs Reply.
xcb_atom_t _xcb_request(struct X11Window* window, const char* name) {
    xcb_intern_atom_cookie_t cookie = xcb_intern_atom(window->connection, 0, strlen(name), name);
    xcb_intern_atom_reply_t* reply = xcb_intern_atom_reply(window->connection, cookie, NULL); // FLUSHES
    if (!reply) return XCB_ATOM_NONE;
    xcb_atom_t atom = reply->atom;
    free(reply);
    return atom;
}

bool platform_initialize() {
    return false;
}

struct WindowConfig* platform_new_window(struct WindowConfig config) {
    // Pass in a generic WindowConfig, return a X11Window that can be cast to WindowConfig vise versa
    struct X11Window* window = (X11Window*) malloc(sizeof(X11Window));
    if (window == NULL) {
        printf("\n>>> Window Malloc Error <<<\n");
        free(window);
        return NULL;
    }
    window->config = config;

    // Connect to the X server
    int screenNum;
    window->connection = xcb_connect(NULL, &screenNum);
    if (xcb_connection_has_error(window->connection)) {
        printf("\n>>> Failed to connect to X Server <<<\n");
        free(window);
        return NULL;
    }
    const xcb_setup_t* setup = xcb_get_setup(window->connection);

    // Get the desired screen
    xcb_screen_iterator_t iter = xcb_setup_roots_iterator(setup);
    for (int i = 0; i < screenNum; i++) {
        xcb_screen_next(&iter);
    }
    window->screen = iter.data;

    window->window = xcb_generate_id(window->connection);

    uint32_t mask = XCB_CW_BACK_PIXEL | XCB_CW_EVENT_MASK;
    uint32_t values[2] = {
        window->screen->black_pixel,
        XCB_EVENT_MASK_EXPOSURE |
        XCB_EVENT_MASK_KEY_PRESS | 
        XCB_EVENT_MASK_STRUCTURE_NOTIFY // RESIZE
    };

    xcb_create_window(
        window->connection,
        XCB_COPY_FROM_PARENT,
        window->window,
        window->screen->root,
        0, 0,
        window->config.window_aspect.width,
        window->config.window_aspect.height,
        0,
        XCB_WINDOW_CLASS_INPUT_OUTPUT,
        window->screen->root_visual,
        mask, values
    );

    _rename_window(window, window->config.window_name);
    
    if (ATOM_WM_PROTOCOLS == XCB_ATOM_NONE) 
        ATOM_WM_PROTOCOLS = _xcb_request(window, "WM_PROTOCOLS");

    if (ATOM_WM_DELETE_WINDOW == XCB_ATOM_NONE)
        ATOM_WM_DELETE_WINDOW = _xcb_request(window, "WM_DELETE_WINDOW");

    // Now an atom will be sent when user hits X so we can close properly.
    xcb_change_property(
        window->connection,
        XCB_PROP_MODE_REPLACE,
        window->window,
        ATOM_WM_PROTOCOLS,
        XCB_ATOM_ATOM,
        32,
        1,
        &ATOM_WM_DELETE_WINDOW
    );
    
    // Send to window manager
    xcb_map_window(window->connection, window->window);
    xcb_flush(window->connection);
    
    // Start engine
    // Loop
    
    return &window->config;
}

bool platform_set_window_name(struct WindowConfig* config, const char* name) {
    X11Window* window = _cast_window(config);
    if (window == NULL) return false;
    
    _rename_window(window, name);
    return xcb_flush(window->connection);
}

bool platform_set_window_size(struct WindowConfig* config, struct Aspect size) {
    X11Window* window = _cast_window(config);
    if (window == NULL) return false;
    
    _resize_window(window, &size);
    return xcb_flush(window->connection);
}

bool platform_set_window_resolution(struct WindowConfig* config, struct Aspect res) {
    // TODO: 
    return false;
}

bool platform_update_window(struct WindowConfig* config) {
    X11Window* window = _cast_window(config);
    if (window == NULL) return false;
    return xcb_flush(window->connection);
}

bool platform_iterate(struct WindowConfig* config) {
    // TODO: Input Event Handling
    return false;
}

bool platform_render(struct WindowConfig* config, float alpha) {
    // TODO: Pass into render.h?
    return false;
}

#endif
