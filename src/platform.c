#include "platform.h"
#include "engine.h"
#include "time.h"
#include "render.h"
#include <stdint.h>
#include <string.h>
#include "color.h"

#define WINDOWS_AMOUNT 1
static WINDOWINFO* window_configs[WINDOWS_AMOUNT];
static size_t window_configs_count = 0;


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

void platform_initialize() {
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
#include <xcb/shm.h>
#include <sys/shm.h>
#include <sys/ipc.h>

#define XCB_EVENT_RESPONSE_TYPE_MASK    ~0x80

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
    printf("here");
    if (!_check_window(window)) return false;
    window->config.window_name = name;
    xcb_change_property(
        window->connection,
        XCB_PROP_MODE_REPLACE,
        window->window,
        XCB_ATOM_WM_NAME, XCB_ATOM_STRING,
        8, strlen(name), name
    );
    printf("here");
    return true;
}

/// Helper function, changes resolution without flushing
bool _update_resolution(struct X11Window* window, struct Aspect res) {
    if (res.width > window->config.window_aspect.width
        || res.height > window->config.window_aspect.height) {
        printf("\n>>> _update_resolution: INVALID RES PARAM <<<\n");
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
        printf("\n>>> _update_resolution : FAILED SHM SETUP <<<\n");
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

/// Helper function to declutter platform_initialize
void _init_loop() {
    xcb_generic_event_t* event;
    for (size_t i = 0; i < window_configs_count; i++) {
        X11Window* window = window_configs[i];
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
                        printf("\n>>> Resized Window <<<\n");
                        printf("\n>>>> WIN: %d, %d < > REN: %d, %d\n",
                               window->config.window_aspect.width,
                               window->config.window_aspect.height,
                               window->config.render_aspect.width,
                               window->config.render_aspect.height);
                        platform_set_window_resolution(window, window->config.window_aspect);
                        printf("\n>>>>> WIN: %d, %d < > REN: %d, %d\n",
                               window->config.window_aspect.width,
                               window->config.window_aspect.height,
                               window->config.render_aspect.width,
                               window->config.render_aspect.height);
                    }
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
        // printf("\n>>> TICK <<<\n");
    }
    engine_close();
}

struct WINDOWINFO* platform_new_window(const char* windowName, struct Aspect windowSize, struct Aspect resolution, float fps) {
    // TODO: fix all the memory leaks when erroring out
    // https://stackoverflow.com/questions/27745131/how-to-use-shm-pixmap-with-xcb
    if (window_configs_count >= EO_WINDOWS_AMOUNT) {
        printf("\n>>> platform_new_window: ATTEMPT TO EXCEED MAX WINDOWS <<<\n");
        return NULL;
    }

    struct X11Window* window = calloc(1, sizeof(struct X11Window));
    if (window == NULL) {
        printf("\n>>> platform_new_window : NULL ALLOC <<<\n");
        return NULL;
    }

    printf("\n>>> Alloc New Window <<<\n");

    window->config = (struct WindowConfig) {
        .window_aspect = windowSize,
        .render_aspect = resolution,
        .window_name = windowName,
        .frames_per_second = fps
    };
    
    // Setup x11 Connection and Window
    window->connection = xcb_connect(NULL, NULL);
    window->screen = xcb_setup_roots_iterator(xcb_get_setup(window->connection)).data;

    printf("\n>>> XCB Connection Setup <<<\n");

    uint32_t window_mask = XCB_CW_EVENT_MASK;
    uint32_t window_values[] = {XCB_EVENT_MASK_EXPOSURE | XCB_EVENT_MASK_STRUCTURE_NOTIFY};
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

    printf("\n>>> XCB Create Window <<<");
    
    // Create a Graphics Context
    window->gc = xcb_generate_id(window->connection);
    uint32_t gc_mask = XCB_GC_FOREGROUND | XCB_GC_GRAPHICS_EXPOSURES;
    uint32_t gc_values[] = {window->screen->black_pixel, 0};
    xcb_create_gc(window->connection, window->gc, window->window, gc_mask, gc_values);

    xcb_map_window(window->connection, window->window);
    xcb_flush(window->connection);

    printf("\n>>> Flushed and Instantiated GC <<<\n");

    // Query SHM availability
    const xcb_query_extension_reply_t* shm_extension = xcb_get_extension_data(window->connection, &xcb_shm_id);
    if (!shm_extension || !shm_extension->present) {
        printf("\n>>> platform_new_window : SHM NOT AVAILABLE <<<\n");
        return NULL;
    }

    printf("\n>>> Passed SHM Availability Check <<<\n");
    
    if (!_update_resolution(window, window->config.render_aspect)) {
        printf("\n>>> platform_new_window : Failed to init resolution <<<\n");
        return NULL;
    }

    if (!_name_window(window, window->config.window_name)) {
        printf("\n>>> platform_new_window : Failed to init name <<<\n");
        return NULL;
    }

    printf("\n>>> Updated Window Paramters ... DONE! <<<\n");
    
    window_configs[window_configs_count] = window;
    window_configs_count++;
    return window;
}

bool platform_set_window_name(struct WINDOWINFO* window, const char* name) {
    printf("here");
    if (!_check_window(window)) {
        printf("\n>>> platform_set_window_name : Failed <<<\n");
        return false;
    }
    printf("here");
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
    // render_draw(&window->config, alpha);
    
    for (int y = 0; y < window->config.render_aspect.height; y++) {
        for (int x = 0; x < window->config.render_aspect.width; x++) {
            window->config.framebuffer[y * window->config.render_aspect.width + x] = rgba(255, 255, 255, 255);
        }
    }
    
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
    return false;
}

void platform_free() {
    for (size_t i = 0; i < window_configs_count; i++) {
        X11Window* window = window_configs[i];
        if (!_check_window(window)) continue;
        
        xcb_shm_detach(window->connection, window->info.shmseg);
        shmdt(window->info.shmaddr);
        shmctl(window->info.shmid, IPC_RMID, 0);
        xcb_free_pixmap(window->connection, window->pixmap);
        xcb_destroy_window(window->connection, window->window);
        xcb_disconnect(window->connection);
        
        if (window->config.world_handler != NULL)
            world_handler_free(window->config.world_handler);

        free(window);
        window_configs[i] = NULL;
    }
    window_configs_count = 0;
}

#endif
