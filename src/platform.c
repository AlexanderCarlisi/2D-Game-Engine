#include "platform.h"
#include "engine.h"
#include "time.h"
#include "render.h"

#define WINDOWS_AMOUNT 1
static WindowConfig* window_configs[WINDOWS_AMOUNT];


#ifdef _WIN32

struct W32Window* _cast_window(struct WindowConfig* config) {
    if (config == NULL) {
        printf("\n>>> _cast_window: NULL argument <<<\n");
        return NULL;
    }
    return (W32Window*) config;
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg) {
        // TODO: handle close and destory gracefully?
        case WM_CLOSE: DestroyWindow(hwnd); return 0;
        case WM_DESTROY: PostQuitMessage(0); return 0;
        case WM_SIZE: {
            // TODO: i think this is wrong?
            // int width = LOWORD(lParam);
            // int height = HIWORD(lParam);

            // if (engine_is_running()) {
            //     appconfig_platform_resized_window_px(width, height);
            // }

            return 0;
        }
        default: return DefWindowProc(hwnd, uMsg, wParam, lParam);
    }
}

bool platform_initialize() {
    return false;
}

struct WindowConfig* platform_new_window(struct WindowConfig config) {
    W32Window* window = (W32Window*) malloc(sizeof(W32Window));
    if (window == NULL) {
        printf("\n>>> W32 failed W32Window Allocation <<<\n");
        return NULL;
    }

    window->config = config;

    WNDCLASS wc = {0};
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = GetModuleHandle(NULL);
    wc.lpszClassName = "WindowClass";

    RegisterClass(&wc);

    window->hwnd = CreateWindowEx(
        0, "WindowClass", window->config.window_name,
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT,
        window->config.window_aspect.width, window->config.window_aspect.height,
        NULL, NULL, wc.hInstance, NULL
    );

    if (!window->hwnd) {
        printf(">>> W32: HWND is null <<<\n");
        return NULL;
    }

    window->hdc = GetDC(window->hwnd);
    platform_set_window_resolution(&window->config, window->config.render_aspect);

    ShowWindow(window->hwnd, SW_SHOW);
    initialize_time_frequency();
    
    // engine start
    // engine loop

    return &window->config;
}

bool platform_set_window_name(struct WindowConfig* config, const char* name) {
    W32Window* window = _cast_window(config);
    if (window == NULL) return false;
    // window->config.window_name = name;
    return SetWindowText(window->hwnd, name);
}

bool platform_set_window_size(struct WindowConfig* config, struct Aspect size) {
    W32Window* window = _cast_window(config);
    if (window == NULL) return false;
    return SetWindowPos(
        window->hwnd,
        NULL,
        0,
        0,
        size.height, size.width,
        SWP_NOMOVE | SWP_NOZORDER
    );
}

bool platform_set_window_resolution(struct WindowConfig* config, struct Aspect res) {
    W32Window* window = _cast_window(config);
    if (window == NULL) return false;

    if (window->bitmap) {
        DeleteObject(window->bitmap);
        window->bitmap = NULL;
    }
    
    // Not a memory leak, framebuffer is owned by DIB
    window->config.framebuffer = NULL;

    memset(&window->bitmapInfo, 0, sizeof(BITMAPINFO));
    window->bitmapInfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    window->bitmapInfo.bmiHeader.biWidth = res.width;
    window->bitmapInfo.bmiHeader.biHeight = -res.height; // top-down DIB
    window->bitmapInfo.bmiHeader.biPlanes = 1;
    window->bitmapInfo.bmiHeader.biBitCount = 32;
    window->bitmapInfo.bmiHeader.biCompression = BI_RGB;

    window->bitmap = CreateDIBSection(
        window->hdc,
        &window->bitmapInfo,
        DIB_RGB_COLORS,
        (void**) &window->config.framebuffer,
        NULL,
        0
    );
    
    // There is a Bitmap that gets assigned automatically cannot be deallocated, 
    // as its within context, so we store its address and free it once the program quits
    if (window->oldBitMap == NULL) {
        window->oldBitMap = SelectObject(window->hdc, window->bitmap);
        return true;
    } else {
        return SelectObject(window->hdc, window->bitmap);
    }
}

bool platform_update_window(struct WindowConfig* config) {
    // TODO: Some kind of windows flush
    return false;
}

bool platform_iterate(struct WindowConfig* config) {
    // TODO: 
    return false;
}

bool platform_render(struct WindowConfig* config, float alpha) {
    // TODO: 
    return false;
}

#else
// TODO: Linux
#include <stdlib.h>
#include <string.h>
#include <sched.h>
#include <stdio.h>
#include <xcb/xcb.h>
#include <xcb/xproto.h>
#include <xcb/xcb_image.h>

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
    // Start engine
    engine_start();
    
    // Loop
    while (engine_is_running()) {

        for (size_t i = 0; i < WINDOWS_AMOUNT; i++) {
            X11Window* window = _cast_window(window_configs[i]);
            xcb_generic_event_t *event;
            while ((event = xcb_poll_for_event(window->connection))) {
                switch (event->response_type & ~0x80) {
                    case XCB_EXPOSE:
                        // Could trigger a redraw
                        break;
                    case XCB_CONFIGURE_NOTIFY: {
                        // Window resized
                        xcb_configure_notify_event_t* cfg = (xcb_configure_notify_event_t*)event;
                        // appconfig_platform_resized_window_px(cfg->width, cfg->height);
                        break;
                    }
                    case XCB_CLIENT_MESSAGE: {
                        xcb_client_message_event_t* cm = (xcb_client_message_event_t*) event;
                        if (cm->data.data32[0] == ATOM_WM_DELETE_WINDOW) {
                            printf("User clicked X, closing window..\n");
                            goto end;
                        }
                    }
                    // case XCB_KEY_PRESS:
                    //     // Could handle keys or just exit
                    //     printf("Key pressed, exiting...\n");
                    //     engine_close();
                    //     free(event);
                    //     goto end;
                }
                free(event);
            }
            engine_tick(&window->config);
        }
    }

end:
    engine_close();
    for (int i = 0; i < WINDOWS_AMOUNT; i++) {
        X11Window* window = _cast_window(window_configs[i]);
        xcb_disconnect(window->connection);
    }
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
    
    window->gc = xcb_generate_id(window->connection);
    mask = XCB_GC_FOREGROUND | XCB_GC_BACKGROUND;
    uint32_t valuesGC[2] = { window->screen->black_pixel, window->screen->white_pixel };
    xcb_create_gc(window->connection, window->gc, window->window, mask, valuesGC);

    platform_set_window_resolution(&window->config, window->config.render_aspect);
    
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
    X11Window* window = _cast_window(config);
    if (window == NULL) return false;
        
    if (window->pixmap != XCB_PIXMAP_NONE) {
        xcb_free_pixmap(window->connection, window->pixmap);
        window->pixmap = XCB_PIXMAP_NONE;
    }

    window->pixmap = xcb_generate_id(window->connection);
    window->config.render_aspect = res;
    xcb_create_pixmap(
        window->connection,
        window->screen->root_depth,
        window->pixmap,
        window->window,
        res.width, res.height
    );
    return true;
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
    struct X11Window* window = _cast_window(config);
    if (window == NULL) return false;
    
    render_draw(config, alpha);
    
    uint16_t width  = window->config.render_aspect.width;
    uint16_t height = window->config.render_aspect.height;
    uint32_t* framebuffer = window->config.framebuffer;

    // Send framebuffer as image
    xcb_put_image(
        window->connection,
        XCB_IMAGE_FORMAT_Z_PIXMAP,
        window->pixmap,
        window->gc,
        width, height,
        0, 0, 0,
        32,
        width * height * 4,
        (uint8_t*)framebuffer
    );

    // Copy to actual window
    xcb_copy_area(
        window->connection,
        window->pixmap,
        window->window,
        window->gc,
        0, 0, 0, 0,
        width, height
    );

    xcb_flush(window->connection);
    return true;
}

#endif
