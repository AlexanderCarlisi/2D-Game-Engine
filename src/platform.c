#include "platform.h"
#include "engine.h"
#include "e_time.h" 
#include "render.h"
#include <stdint.h>
#include <string.h>
#include "logger.h"
#include "input.h"
#include "color.h"

static AWINDOW* window_configs[EO_WINDOWS_AMOUNT];
static size_t window_configs_count = 0;

#ifdef _WIN32


LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg) {
        case WM_DESTROY: {
            PostQuitMessage(0);
            return 0;
        }

        case WM_PAINT: {
            // IM HANDLING IT STOP YELLING AT ME
            ValidateRect(hwnd, NULL);
            return 0;
        }

        // case WM_SIZE: {
        //     int width = LOWORD(lParam);
        //     int height = HIWORD(lParam);
        // }
    }
    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

struct W32Window* platform_new_window(const char* windowName, struct Aspect windowSize, struct Aspect resolution, float fps) {
    if (window_configs_count >= EO_WINDOWS_AMOUNT) {
        logger_write(1, 0, "platform_new_window: attempted to exceed max windows allocated", true);
        return NULL;
    }

    struct W32Window* window = calloc(1, sizeof(struct W32Window));
    if (window == NULL) {
        logger_write(1, 0, "platform_new_window: Window alloc failed", true);
        return NULL;
    }

    logger_write(1, 0, "Allocated new Window", false);

    window->config = (struct WindowConfig) {
        .window_aspect = windowSize,
        .render_aspect = resolution,
        .window_name = windowName,
        .frames_per_second = fps
    };

    char CLASS_NAME[12 + (EO_WINDOWS_AMOUNT / 10)]; // USE ANSII (12 is CLASS_NAME-\0)
    snprintf(CLASS_NAME, sizeof(CLASS_NAME), "CLASS_NAME-%d", window_configs_count);

    WNDCLASS wc = {0};
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = GetModuleHandle(NULL); // This works
    wc.lpszClassName = CLASS_NAME;

    RegisterClass(&wc);

    HWND hwnd = CreateWindowEx(
        0,
        CLASS_NAME,
        windowName,
        WS_OVERLAPPEDWINDOW, // window style

        // Size and Position
        CW_USEDEFAULT, CW_USEDEFAULT,
        windowSize.width, windowSize.height,
        NULL, // PARENT
        NULL, // HMENU
        wc.hInstance,
        NULL
    );

    if (hwnd == NULL) {
        return 0;
    }

    window->hwnd = hwnd;
    
    platform_set_window_resolution(window, resolution);
    ShowWindow(hwnd, SW_SHOWDEFAULT);

    logger_write(1, 0, "Updated Window Paramters", false);
    window_configs[window_configs_count] = window;
    window_configs_count++;
    return window;
}

bool platform_update_window(struct AWINDOW* window) {
    return false;
}

bool platform_set_window_name(struct AWINDOW* window, const char* name) {
    return false;
}

bool platform_set_window_size(struct AWINDOW* window, struct Aspect size) {
    return false;
}

bool platform_set_window_resolution(struct AWINDOW* window, struct Aspect res) {
    window->config.render_aspect = res;

    BITMAPINFO bmi = {0};
    bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bmi.bmiHeader.biWidth = window->config.render_aspect.width;
    bmi.bmiHeader.biHeight = -window->config.render_aspect.height; // Negative for top-down
    bmi.bmiHeader.biPlanes = 1;
    bmi.bmiHeader.biBitCount = 32;
    bmi.bmiHeader.biCompression = BI_RGB;
    window->bitmapInfo = bmi;

    if (window->config.framebuffer != NULL) {
        free(window->config.framebuffer);
    }
    window->config.framebuffer = malloc(res.height * res.width * sizeof(uint32_t));

    return true;
}

bool platform_render(struct AWINDOW* window, float alpha) {
    // printf("here\n\n");
    render_clear(&window->config, EO_RENDER_CLEAR);
    // printf("here\n\n");
    // render_draw(&window->config, alpha);
    render_test(&window->config);
    // printf("here\n\n");

    HDC hdc = GetDC(window->hwnd);
    RECT rect;
    GetClientRect(window->hwnd, &rect);
    int window_width = rect.right - rect.left;
    int window_height = rect.bottom - rect.top;

    // copies FB into GPU every frame, this is high key fucking ass
    StretchDIBits(
        hdc,
        0, 0, window_width, window_height,
        0, 0, window->config.render_aspect.width,
        window->config.render_aspect.height,
        window->config.framebuffer,
        &window->bitmapInfo,
        DIB_RGB_COLORS,
        SRCCOPY
    );

    ReleaseDC(window->hwnd, hdc);
    return true;
}

bool platform_iterate(struct AWINDOW* window) {
    world_handler_update_active(window->config.world_handler);
    return true;
}

void platform_free() {

}

/// https://learn.microsoft.com/en-us/windows/win32/learnwin32/window-messages
/// https://learn.microsoft.com/en-us/windows/win32/winmsg/about-messages-and-message-queues
/// Why are the windows docs kinda peak tho
void platform_initialize() {
    initialize_time_frequency();
    engine_start();
    // bool bRet;
    MSG msg = { };
    while (engine_is_running()) {
        // Messages are sent to the Application
        // Messages reference the Window Handle they are responding to
        // Therefore messages have their own check loop
        // while ((bRet = PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) > 0) {
        while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
            // Message error handling
            // Peek doesnt do that, only Get, whoops
            // if (bRet == -1) {
            //     logger_write(1, 0, "Windows MSG Err", true);
            //     engine_set_running(false);
            //     break;
            // }

            if (msg.message == WM_QUIT) {
                logger_write(1, 0, "App Close", false);
                engine_set_running(false);
                break;
            }

            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }

        // Now loop through Engine logic for each window
        for (size_t i = 0; i < window_configs_count; i++) {
            // printf("here\n\n");
            W32Window* window = window_configs[i];
            if (window == NULL) continue;
            engine_tick(window);
        }
    }
    // printf("here\n\n");
    engine_close();
}

#else
#include <stdlib.h>
#include <string.h>
#include <sched.h>
#include <stdio.h>
#include <xcb/xcb.h>
#include <xcb/xproto.h>
#include <xcb/xcb_image.h>
#include <xcb/shm.h>
#include <sys/shm.h>
#include <sys/ipc.h>

#define XCB_EVENT_RESPONSE_TYPE_MASK    ~0x80

xcb_atom_t ATOM_WM_PROTOCOLS = XCB_ATOM_NONE;
xcb_atom_t ATOM_WM_DELETE_WINDOW = XCB_ATOM_NONE;

bool _check_shm(struct xcb_shm_segment_info_t* info) {
    return info != NULL
        && info->shmaddr != NULL
        && info->shmid >= 0
        && info->shmaddr != NULL
        && info->shmaddr != (void*) -1;
}

/// Helper function, dont segfault
bool _check_window(struct X11Window* window) {
    
    return window != NULL
        && window->connection != NULL
        && window->window != XCB_ATOM_NONE
        && window->screen != NULL
        && window->gc != XCB_ATOM_NONE
        && _check_shm(&window->info);
}

/// Helper function, names without flushing
bool _name_window(struct X11Window* window, const char* name) {
    if (!_check_window(window)) return false;
    window->config.window_name = name;
    xcb_change_property(
        window->connection,
        XCB_PROP_MODE_REPLACE,
        window->window,
        XCB_ATOM_WM_NAME, XCB_ATOM_STRING,
        8, strlen(name), name
    );
    return true;
}

/// Helper function, changes resolution without flushing
bool _update_resolution(struct X11Window* window, struct Aspect res) {
    if (res.width > window->config.window_aspect.width
        || res.height > window->config.window_aspect.height) {
        logger_write(2, 1, "_update_resolution: Invalid Res Param", true);
        return false;
    }
    window->config.render_aspect = res;

    // Delloc old SHM
    if (_check_shm(&window->info)) {
        xcb_shm_detach(window->connection, window->info.shmseg);
        shmdt(window->info.shmaddr);
        shmctl(window->info.shmid, IPC_RMID, 0);
        xcb_free_pixmap(window->connection, window->pixmap);
    }

    // Allocate SHM
    window->info.shmid = shmget(
        IPC_PRIVATE,
        4 * window->config.render_aspect.width * window->config.render_aspect.height,
        IPC_CREAT | 0600
    );
    
    // Attach Segment to Server
    window->info.shmaddr = shmat(window->info.shmid, 0, 0); // framebuffer addr
    window->info.shmseg = xcb_generate_id(window->connection);
    xcb_shm_attach(window->connection, window->info.shmseg, window->info.shmid, 0);
    
    // Create Pixmap
    window->config.framebuffer = (uint32_t*) window->info.shmaddr;
    window->pixmap = xcb_generate_id(window->connection);
    xcb_shm_create_pixmap(
        window->connection,
        window->pixmap,
        window->window,
        window->config.render_aspect.width,
        window->config.render_aspect.height,
        window->screen->root_depth,
        window->info.shmseg,
        0
    );
    
    if (!_check_shm(&window->info)) {
        logger_write(2, 1, "_update_resolution: Failed SMH Setup", true);
        return false;
    }

    return true;
}

/// Helper function, resizes window, handles res safely, no flush
bool _resize_window(struct X11Window* window, struct Aspect size) {
    if (!_check_window(window)) return false;
    
    // If theres a conflict, set Res to Window Size
    if (window->config.render_aspect.height > size.height 
        || window->config.render_aspect.width > size.width) {
        window->config.render_aspect.width = size.width;
        window->config.render_aspect.height = size.height;
        _update_resolution(window, window->config.render_aspect);
    }

    window->config.window_aspect = size;

    xcb_configure_window(
        window->connection,
        window->window,
        XCB_CONFIG_WINDOW_WIDTH | XCB_CONFIG_WINDOW_HEIGHT,
        (uint32_t[]){size.width, size.height}
    );

    return true;
}

/// Helper function for obtaining atoms. Flushes
xcb_atom_t _xcb_request(struct X11Window* window, const char* name) {
    xcb_intern_atom_cookie_t cookie = xcb_intern_atom(window->connection, 0, strlen(name), name);
    xcb_intern_atom_reply_t* reply = xcb_intern_atom_reply(window->connection, cookie, NULL); // flush
    if (!reply) return XCB_ATOM_NONE;
    xcb_atom_t atom = reply->atom;
    free(reply);
    return atom;
}

/// Helper function for freeing windows.
void _free_window(struct X11Window* window) {
    if (window == NULL) return; // nothing to dealloc
    
    if (window->info.shmseg != XCB_ATOM_NONE) {
        xcb_shm_detach(window->connection, window->info.shmseg);
        logger_write(2, 1, "detach shmseg", false);
    }
    
    if (window->info.shmaddr != NULL) {
        shmdt(window->info.shmaddr);
        logger_write(2, 1, "dealloc shmaddr", false);
    }
    
    if (window->info.shmid != XCB_ATOM_NONE) {
        shmctl(window->info.shmid, IPC_RMID, 0);
        logger_write(2, 1, "shmid ctl", false);
    }
    
    if (window->pixmap != XCB_PIXMAP_NONE) {
        xcb_free_pixmap(window->connection, window->pixmap);
        logger_write(2, 1, "pixmap dealloc", false);
    }
    
    if (window->window != XCB_ATOM_NONE) {
        xcb_destroy_window(window->connection, window->window);
        logger_write(2, 1, "Destroy Window", false);
    }
    
    if (window->connection != NULL) {
        xcb_disconnect(window->connection);
        logger_write(2, 1, "Connection Disconnected", false);
    }
    
    if (window->config.world_handler != NULL) {
        world_handler_free(window->config.world_handler);
        logger_write(2, 1, "Deallocated world_handler", false);
    }
    
    free(window);
    logger_write(1, 0, "_free_window: dealloc finish", false);
}

void _free_window_i(size_t i) {
    if (i < window_configs_count && window_configs[i] != NULL) {
        logger_write(2, 1, "_free_window_i: Dealloc %zu", false);
        _free_window(window_configs[i]);
        window_configs[i] = NULL;
    }
}

/// Helper function to declutter platform_initialize
void _init_loop() {
    xcb_generic_event_t* event;
    for (size_t i = 0; i < window_configs_count; i++) {
        X11Window* window = window_configs[i];
        if (window == NULL) continue;
        
        while ((event = xcb_poll_for_event(window->connection))) {
            switch(event->response_type & XCB_EVENT_RESPONSE_TYPE_MASK) {
                case XCB_CONFIGURE_NOTIFY: {
                    // Window Resized
                    xcb_configure_notify_event_t* cfg = (xcb_configure_notify_event_t*) event;
                    if (cfg->window == window->window) {
                        window->config.window_aspect.width = cfg->width;
                        window->config.window_aspect.height = cfg->height;

                        // TODO: Update this to be NON FLUSHING, since many updates can occur
                        // only update the resolution once the user has stopped changing window
                        // size for a given time.
                        
                        logger_write(2, 1, "Resized Window", false);
                        platform_set_window_resolution(window, window->config.window_aspect);
                    }
                }

                case XCB_CLIENT_MESSAGE: {
                    xcb_client_message_event_t* cm = (xcb_client_message_event_t*) event;
                    if (cm->data.data32[0] == ATOM_WM_DELETE_WINDOW) {
                        logger_write(2, 1, "DELETE WINDOW", false);
                        if (i == 0) {
                            // Shutdown
                            engine_set_running(false);
                            logger_write(2, 1, "Engine Off", false);
                        } else {
                            _free_window_i(i);
                            logger_write(2, 1, "Dealloc %zu", false);
                            printf("\n>>> _init_loop : Dealloc %zu <<<\n", i);
                        }
                    }
                    break;
                }

                case XCB_KEY_PRESS: {
                    xcb_key_press_event_t* kp = (xcb_key_press_event_t*) event;
                    input_kb_set_press(kp->detail);
                    
                    // logger_write(1, 1, "KeyCode: %d", kp->detail);
                    // printf("Key Press: %d\n", kp->detail);
                    break;
                }

                case XCB_KEY_RELEASE: {
                    xcb_key_release_event_t* kr = (xcb_key_release_event_t*) event;
                    input_kb_set_release(kr->detail);
                    
                    // printf("Key Release: %d\n", kr->detail);
                    break;
                }
            }
            free(event);
        }
        engine_tick(window); // Render, Physics, Inputs, Game
    } 
}

void platform_initialize() {
    engine_start();
    while (engine_is_running()) {
        _init_loop();
    }
    engine_close();
}

struct AWINDOW* platform_new_window(const char* windowName, struct Aspect windowSize, struct Aspect resolution, float fps) {
    // XCB-SHM https://stackoverflow.com/questions/27745131/how-to-use-shm-pixmap-with-xcb
    // WM_PROTO and DELETE WINDOW https://marc.info/?l=freedesktop-xcb&m=129381953404497
    if (window_configs_count >= EO_WINDOWS_AMOUNT) {
        logger_write(1, 0, "platform_new_window: attempted to exceed max windows allocated", true);
        return NULL;
    }

    struct X11Window* window = calloc(1, sizeof(struct X11Window));
    if (window == NULL) {
        logger_write(1, 0, "platform_new_window: Window alloc failed", true);
        return NULL;
    }

    logger_write(1, 0, "Allocated new Window", false);

    window->config = (struct WindowConfig) {
        .window_aspect = windowSize,
        .render_aspect = resolution,
        .window_name = windowName,
        .frames_per_second = fps
    };
    
    // Setup x11 Connection and Window
    window->connection = xcb_connect(NULL, NULL);
    if (window->connection == NULL) {
        logger_write(1, 0, "platform_new_window: X11 connection failed", true);
        _free_window(window);
        return NULL;
    }

    window->screen = xcb_setup_roots_iterator(xcb_get_setup(window->connection)).data;
    if (window->screen == NULL) {
        logger_write(1, 0, "platform_new_window: XCB screen iteration failed", true);
        _free_window(window);
        return NULL;
    }

    logger_write(1, 0, "X11 connection instantiated", false);

    uint32_t window_mask = XCB_CW_EVENT_MASK;
    uint32_t window_values[] = {
        XCB_EVENT_MASK_EXPOSURE 
        | XCB_EVENT_MASK_STRUCTURE_NOTIFY
        | XCB_EVENT_MASK_KEY_PRESS
        | XCB_EVENT_MASK_KEY_RELEASE
    };
    window->window = xcb_generate_id(window->connection);
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
        window_mask, window_values
    );

    logger_write(1, 0, "X11 Window instantiated", false);
    
    // Create a Graphics Context
    window->gc = xcb_generate_id(window->connection);
    uint32_t gc_mask = XCB_GC_FOREGROUND | XCB_GC_GRAPHICS_EXPOSURES;
    uint32_t gc_values[] = {window->screen->black_pixel, 0};
    xcb_create_gc(window->connection, window->gc, window->window, gc_mask, gc_values);

    xcb_map_window(window->connection, window->window);
    xcb_flush(window->connection);

    logger_write(1, 0, "Flushed and Instantiated GC", false);

    // Setup extra contexts
    if (ATOM_WM_PROTOCOLS == XCB_ATOM_NONE)
        ATOM_WM_PROTOCOLS = _xcb_request(window, "WM_PROTOCOLS");
    
    if (ATOM_WM_DELETE_WINDOW == XCB_ATOM_NONE)
        ATOM_WM_DELETE_WINDOW = _xcb_request(window, "WM_DELETE_WINDOW");
    
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

    // Query SHM availability
    const xcb_query_extension_reply_t* shm_extension = xcb_get_extension_data(window->connection, &xcb_shm_id);
    if (!shm_extension || !shm_extension->present) {
        logger_write(1, 0, "platform_new_window: SHM not availible", true);
        _free_window(window);
        return NULL;
    }

    logger_write(1, 0, "Passed SHM availability check", false);
    
    if (!_update_resolution(window, window->config.render_aspect)) {
        logger_write(1, 0, "platform_new_window: Failed to init resolution", true);
        _free_window(window);
        return NULL;
    }

    if (!_name_window(window, window->config.window_name)) {
        logger_write(1, 0, "platform_new_window: Failed to init name", true);
        _free_window(window);
        return NULL;
    }

    logger_write(1, 0, "Updated Window Paramters", false);
    
    window_configs[window_configs_count] = window;
    window_configs_count++;
    return window;
}

bool platform_set_window_name(struct AWINDOW* window, const char* name) {
    if (!_check_window(window)) {
        printf("\n>>> platform_set_window_name : Failed <<<\n");
        logger_write(1, 0, "platform_set_window_name: failed", true);
        return false;
    }
    _name_window(window, name);
    return xcb_flush(window->connection);
}

bool platform_set_window_size(struct X11Window* window, struct Aspect size) {
    if (!_check_window(window)) return false;
    _resize_window(window, size);
    return xcb_flush(window->connection);
}

bool platform_set_window_resolution(struct X11Window* window, struct Aspect res) {
    if (!_check_window(window)) return false;
    _update_resolution(window, res);
    return xcb_flush(window->connection);
}

bool platform_update_window(struct X11Window* window) {
    if (!_check_window(window)) return false;

    bool status = _update_resolution(window, window->config.render_aspect);
    if (!status) return false;
    status = _resize_window(window, window->config.window_aspect);
    if (!status) return false;
    status = _name_window(window, window->config.window_name);
    if (!status) return false;

    return xcb_flush(window->connection);
}

bool platform_render(struct X11Window* window, float alpha) {
    render_clear(&window->config, EO_RENDER_CLEAR);
    render_draw(&window->config, alpha);
    
    // xcb_shm_put_image(
    //     window->connection,
    //     window->pixmap,          // destination drawable
    //     window->gc,
    //     window->config.render_aspect.width,
    //     window->config.render_aspect.height,
    //     0,                       // dst_x
    //     0,                       // dst_y
    //     window->config.render_aspect.width,
    //     window->config.render_aspect.height,
    //     0,                       // src_x
    //     0,                       // src_y
    //     32,                      // depth (match your pixmap depth)
    //     XCB_IMAGE_FORMAT_Z_PIXMAP,
    //     0, // uint8_t sendevent
    //     window->info.shmseg,          // shared memory segment
    //     0                        // offset into SHM
    // );
    
    // Copy pixmap to Window
    xcb_copy_area(
        window->connection,
        window->pixmap,
        window->window,
        window->gc,
        0, 0, 0, 0,
        window->config.render_aspect.width,
        window->config.render_aspect.height
    );

    return xcb_flush(window->connection);
}

bool platform_iterate(struct X11Window* window) {
    world_handler_update_active(window->config.world_handler);
    return true;
}

void platform_free() {
    for (size_t i = 0; i < window_configs_count; i++) {
        _free_window_i(i);
    }
    window_configs_count = 0;
}

#endif
