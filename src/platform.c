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
#include <stdlib.h>
#include <stdio.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <xcb/xcb.h>
#include <xcb/shm.h>
#include <xcb/xcb_image.h>

#if __ORDER_LITTLE_ENDIAN == __BYTE_ORDER__
# define NATIVE_XCB_IMAGE_ORDER    XCB_IMAGE_ORDER_LSB_FIRST
#else
# define NATIVE_XCB_IMAGE_ORDER    XCB_IMAGE_ORDER_MSB_FIRST
#endif

static xcb_format_t const *query_xcb_format_for_depth(
	xcb_connection_t *const connection,
	unsigned depth )
{
	xcb_setup_t const *const setup = xcb_get_setup(connection);
	xcb_format_iterator_t it;
	for( it = xcb_setup_pixmap_formats_iterator(setup)
	   ; it.rem
	   ; xcb_format_next(&it)
	){
		xcb_format_t const *const format = it.data;
		if( format->depth == depth ){
			return format;
		}
	}
	return NULL;
}

void _whateverthefuckthisbullshitisdotjpg(struct X11Window* window) {
	xcb_generic_error_t *error = NULL;
	struct Aspect res = window->config.render_aspect;

	xcb_format_t const *const format = query_xcb_format_for_depth(window->connection, 24);
	window->image = xcb_image_create(
		// Obvious
		res.width, res.height,

		// just format bullshit
		XCB_IMAGE_FORMAT_Z_PIXMAP,
		format->scanline_pad,
		format->depth, format->bits_per_pixel, 0,
		
		// Makes sense
		NATIVE_XCB_IMAGE_ORDER,
		XCB_IMAGE_ORDER_MSB_FIRST,
		
		NULL, ~0, 0 // WHAT THE FUCK IS THIS???
	);
	if( !window->image ){
		fprintf(stderr, "could not create X11 image structure\n");
	}

	// What the fuck is a stride, FANCY WIDTH???
	size_t const image_segment_size = window->image->stride * window->image->height;
	
	// some bullshit
	window->shm_id = shmget(IPC_PRIVATE, image_segment_size, IPC_CREAT | 0600);
	if( 0 > window->shm_id ){ goto fail; }
	
	// use some bullshit to GET data?? I thought we was overwriting data
	window->image->data = shmat(window->shm_id, 0, 0);
	if( (void*)-1 == (void*)(window->image->data) ){ goto fail; }
	
	// generate segs id?
	window->shm_seg = xcb_generate_id(window->connection),
	error = xcb_request_check(window->connection,
		xcb_shm_attach_checked(
			window->connection,
			window->shm_seg, window->shm_id, 0) );

// Fall back for external displays?
fail:
	if( window ){
		if( window->image ){
			if( window->image->data && error ){
				shmdt(window->image->data);
				window->image->data = NULL;
			}
			if( !window->image->data ){
				shmctl(window->shm_id, IPC_RMID, 0);
				window->shm_id = -1;
			}
		}
		if( 0 > window->shm_id ){
			xcb_image_destroy(window->image);
			window->image = NULL;
		}
		if( !window->image ){
			free(window);
			window = NULL;
		}
	}
	free(error);
}

struct WINDOWINFO* platform_new_window(
        char* windowName, 
        struct Aspect windowSize, 
        struct Aspect resolution, 
        float fps
    ) {
    xcb_connection_t* connection = xcb_connect(NULL, NULL);

    // check that X MIT-SHM is available
    const xcb_query_extension_reply_t* shm_extension = xcb_get_extension_data(connection, &xcb_shm_id);
    if (!shm_extension || !shm_extension->present) {
        printf("shit dont work");
        return NULL;
    }
    
    struct X11Window* window = calloc(1, sizeof(X11Window));
    window->connection = connection;
    window->config = (struct WindowConfig) {
        .frames_per_second = fps,
        .window_aspect = windowSize,
        .render_aspect = resolution
    };

    _whateverthefuckthisbullshitisdotjpg(window);
    
	window->screen = xcb_setup_roots_iterator(xcb_get_setup(connection)).data;
    
    uint32_t window_mask = XCB_CW_EVENT_MASK;
    uint32_t window_values[] = {XCB_EVENT_MASK_EXPOSURE};
    window->window = xcb_generate_id(window->connection);
    xcb_create_window(
        window->connection,
        XCB_COPY_FROM_PARENT,
        window->window,
        window->screen->root,
        0, 0,
        windowSize.width, windowSize.height,
        0,
        XCB_WINDOW_CLASS_INPUT_OUTPUT,
        window->screen->root_visual,
        window_mask, window_values
    );

    window->gc = xcb_generate_id(window->connection);
    uint32_t gc_mask = XCB_GC_FOREGROUND | XCB_GC_GRAPHICS_EXPOSURES;
    uint32_t gc_values[] = {window->screen->black_pixel, 0};
    xcb_create_gc(window->connection, window->gc, window->window, gc_mask, gc_values);

    xcb_map_window(window->connection, window->window);
    xcb_flush(window->connection);
    return window;
}

// Atoms are global to the X Server instance
// xcb_atom_t ATOM_WM_PROTOCOLS = XCB_NONE;
// xcb_atom_t ATOM_WM_DELETE_WINDOW = XCB_NONE;
//
// /// @brief attempt to cast WindowConfig to X11Window.
// /// @return X11Window_ptr, or NULL on fail.
// struct X11Window* _cast_window(struct WindowConfig* windowConfig) {
//     if (windowConfig == NULL) {
//         printf("\n>>> _cast_window: NULL Pointer argument <<<\n");
//         return NULL;
//     }
//
//     return (X11Window*) windowConfig;
// }
//
// /// @brief XCB Window Resize Helper function, doesn't flush.
// void _resize_window(struct X11Window* window, struct Aspect* windowSize) {
//     if (!window->connection) return;
//     xcb_configure_window(
//         window->connection,
//         window->window,
//         XCB_CONFIG_WINDOW_WIDTH | XCB_CONFIG_WINDOW_HEIGHT,
//         (uint32_t[]){windowSize->width, windowSize->height}
//     );
// }
//
// /// @brief XCB Window Rename Helper function, doesn't flush.
// void _rename_window(struct X11Window* window, const char* name) {
//     if (!window->connection) return;
//
//     xcb_change_property(
//         window->connection,
//         XCB_PROP_MODE_REPLACE,
//         window->window,
//         XCB_ATOM_WM_NAME, XCB_ATOM_STRING,
//         8, strlen(name), name
//     );
// }
//
// /// @brief Request an atom from the X11 instance using XCB.
// /// @return The requested atom response. 
// /// @note Flushes, and Blocking. Deallocs Reply.
// xcb_atom_t _xcb_request(struct X11Window* window, const char* name) {
//     xcb_intern_atom_cookie_t cookie = xcb_intern_atom(window->connection, 0, strlen(name), name);
//     xcb_intern_atom_reply_t* reply = xcb_intern_atom_reply(window->connection, cookie, NULL); // FLUSHES
//     if (!reply) return XCB_ATOM_NONE;
//     xcb_atom_t atom = reply->atom;
//     free(reply);
//     return atom;
// }
//

void platform_initialize() {
    // Start engine
    engine_start();

    // Loop
    while (engine_is_running()) {
        for (size_t i = 0; i < window_configs_count; i++) {
            X11Window* window = window_configs[i];
            xcb_generic_event_t *event;
            while ((event = xcb_poll_for_event(window->connection))) {
            //     switch (event->response_type & ~0x80) {
            //         // case XCB_EXPOSE:
            //         //     // Could trigger a redraw
            //         //     break;
            //         // case XCB_CONFIGURE_NOTIFY: {
            //         //     // Window resized
            //         //     xcb_configure_notify_event_t* cfg = (xcb_configure_notify_event_t*)event;
            //         //     // platform_set_window_resolution(&window->config, (struct Aspect) {.width=cfg->width, .height=cfg->height});
            //         //     break;
            //         // }
            //         // case XCB_CLIENT_MESSAGE: {
            //         //     xcb_client_message_event_t* cm = (xcb_client_message_event_t*) event;
            //         //     if (cm->data.data32[0] == ATOM_WM_DELETE_WINDOW) {
            //         //         printf("User clicked X, closing window..\n");
            //         //         goto end;
            //         //     }
            //         //     break;
            //         // }
            //         // case XCB_KEY_PRESS: {
            //         //     // Could handle keys or just exit
            //         //     printf("Key pressed, exiting...\n");
            //         //     // engine_close();
            //         //     // goto end;
            //         //     break;
            //         // }
            //     }
            //     free(event);
            }
            engine_tick(window);
        }
    }

end:
    // engine_close();
    platform_free();
}
//
// struct WindowConfig* platform_new_window(
//         char* windowName,
//         struct Aspect windowSize,
//         struct Aspect resolution,
//         float fps
//     ) {
//
//     // Don't index out of bounds, you should know how many windows you want.
//     // Else, rewrite the engine yourself for a dynamic amount
//     if (window_configs_count >= WINDOWS_AMOUNT) {
//         printf("\n>>> Out of Space for WindowConfigs <<<\n");
//         return NULL;
//     }
//
//     // Dynamically allocate a X11Window Instance
//     struct X11Window* window = (X11Window*) malloc(sizeof(X11Window));
//     if (window == NULL) {
//         printf("\n>>> Window Malloc Error <<<\n");
//         return NULL;
//     }
//     printf("\n>>> Allocated Window <<<\n");
//
//     // Setup Config
//     window->config = (struct WindowConfig) {
//         .window_aspect = windowSize,
//         .render_aspect = resolution,
//         .frames_per_second = fps
//     };
//     strcpy(window->config.window_name, windowName);
//     // printf("\n>>> strcopied <<<\n");
//
//     // Connect to the X server
//     int screenNum;
//     window->connection = xcb_connect(NULL, &screenNum);
//     if (xcb_connection_has_error(window->connection)) {
//         printf("\n>>> Failed to connect to X Server <<<\n");
//         free(window);
//         return NULL;
//     }
//     const xcb_setup_t* setup = xcb_get_setup(window->connection);
//     printf("\n>>> settedup <<<\n");
//
//     // Get the desired screen
//     xcb_screen_iterator_t iter = xcb_setup_roots_iterator(setup);
//     for (int i = 0; i < screenNum; i++) {
//         xcb_screen_next(&iter);
//     }
//     window->screen = iter.data;
//
//     window->window = xcb_generate_id(window->connection);
//
//     uint32_t mask = XCB_CW_BACK_PIXEL | XCB_CW_EVENT_MASK;
//     uint32_t values[2] = {
//         window->screen->black_pixel,
//         XCB_EVENT_MASK_EXPOSURE |
//         XCB_EVENT_MASK_KEY_PRESS | 
//         XCB_EVENT_MASK_STRUCTURE_NOTIFY // RESIZE
//     };
//
//     xcb_create_window(
//         window->connection,
//         XCB_COPY_FROM_PARENT,
//         window->window,
//         window->screen->root,
//         0, 0,
//         window->config.window_aspect.width,
//         window->config.window_aspect.height,
//         0,
//         XCB_WINDOW_CLASS_INPUT_OUTPUT,
//         window->screen->root_visual,
//         mask, values
//     );
//
//     printf("\n>>> creatded bindow <<<\n");
//
//     window->gc = xcb_generate_id(window->connection);
//     uint32_t gc_mask = XCB_GC_FOREGROUND | XCB_GC_BACKGROUND | XCB_GC_GRAPHICS_EXPOSURES;
//     uint32_t valuesGC[2] = { window->screen->white_pixel, window->screen->black_pixel };
//     xcb_create_gc(window->connection, window->gc, window->window, gc_mask, valuesGC);
//     printf("\n>>> gc <<<\n");
//
//     window->config.framebuffer = malloc(resolution.width * resolution.height * sizeof(uint32_t));
//     memset(window->config.framebuffer, 0, resolution.width * resolution.height * sizeof(uint32_t));
//
//     // Send to window manager
//     xcb_map_window(window->connection, window->window);
//     xcb_flush(window->connection);
//
//     // if (ATOM_WM_PROTOCOLS == XCB_ATOM_NONE) 
//     //     ATOM_WM_PROTOCOLS = _xcb_request(window, "WM_PROTOCOLS");
//     //
//     // if (ATOM_WM_DELETE_WINDOW == XCB_ATOM_NONE)
//     //     ATOM_WM_DELETE_WINDOW = _xcb_request(window, "WM_DELETE_WINDOW");
//     //
//     // // Now an atom will be sent when user hits X so we can close properly.
//     // xcb_change_property(
//     //     window->connection,
//     //     XCB_PROP_MODE_REPLACE,
//     //     window->window,
//     //     ATOM_WM_PROTOCOLS,
//     //     XCB_ATOM_ATOM,
//     //     32,
//     //     1,
//     //     &ATOM_WM_DELETE_WINDOW
//     // );
//     // printf("\n>>> deletewindow <<<\n");
//
//
//     printf("\n>>> map and flush <<<\n");
//
//
//     // platform_set_window_resolution(&window->config, window->config.render_aspect);
//     printf("\n>>> res <<<\n");
//
//     window_configs[window_configs_count] = (WindowConfig*) window;
//     window_configs_count++;
//     return window_configs[window_configs_count-1];
// }
//
// bool platform_set_window_name(struct WindowConfig* config, const char* name) {
//     X11Window* window = _cast_window(config);
//     if (window == NULL) return false;
//
//     _rename_window(window, name);
//     return xcb_flush(window->connection);
// }
//
// bool platform_set_window_size(struct WindowConfig* config, struct Aspect size) {
//     X11Window* window = _cast_window(config);
//     if (window == NULL) return false;
//
//     _resize_window(window, &size);
//     window->config.window_aspect = size;
//     return xcb_flush(window->connection);
// }
//
// bool platform_set_window_resolution(struct WindowConfig* config, struct Aspect res) {
//     X11Window* window = _cast_window(config);
//     if (window == NULL) return false;
//
//     if (window->config.framebuffer != NULL) {
//         free(window->config.framebuffer);
//         window->config.framebuffer = NULL;
//     }
//
//     window->config.framebuffer = malloc(res.width * res.height * sizeof(uint32_t));
//     memset(window->config.framebuffer, 0, res.width * res.height * sizeof(uint32_t));
//
//     if (window->pixmap != XCB_PIXMAP_NONE) {
//         xcb_free_pixmap(window->connection, window->pixmap);
//         window->pixmap = XCB_PIXMAP_NONE;
//     }
//
//     window->pixmap = xcb_generate_id(window->connection);
//     window->config.render_aspect = res;
//     xcb_create_pixmap(
//         window->connection,
//         window->screen->root_depth,
//         window->pixmap,
//         window->window,
//         res.width, res.height
//     );
//
//     printf("\n>>> NEW RES: %d, %d <<<\n", res.width, res.height);
//     return true;
// }
//
// bool platform_update_window(struct WindowConfig* config) {
//     X11Window* window = _cast_window(config);
//     if (window == NULL) return false;
//     return xcb_flush(window->connection);
// }
//
bool platform_iterate(struct WINDOWINFO* config) {
    // TODO: Input Event Handling
    return false;
}
//
bool platform_render(WINDOWINFO* window, float alpha) {
    if (window == NULL) {
        printf("\n>>> platform_render: CONFIG NULL <<<\n");
        return false;
    }

    // render_clear(&window->config, rgba(100, 100, 100, 100));
    // render_draw(config, alpha);
    
	for( unsigned j = 0; j < window->image->height; ++j ){
		uint8_t *const line = window->image->data + j * window->image->stride;
		for( unsigned i = 0; i  < window->image->width; ++i ){
			unsigned const bytes_per_pixel = window->image->bpp/8;
			uint8_t *pixel = line + i * bytes_per_pixel;
            
            unsigned t = 0xFFFFFF;
            const size_t a = (i + t);
            const size_t b = (j + (i >> 8) & 0xFF);
            const size_t c = (j >> 8) & 0xFF;

            // const size_t rgb = 0xFFFFFF;

            switch (bytes_per_pixel) {
                case 4: pixel[3] = 0xFF;
                case 3: pixel[2] = a & 0xFF;
                case 2: pixel[1] = b & 0xFF;
                case 1: pixel[0] = c & 0xFF;
                default: break;
            }
        }
    }

    xcb_shm_put_image(
        window->connection,
        window->window,
        window->gc,
        window->image->width, window->image->height, 0, 0,
        window->image->width, window->image->height, 0, 0,
        window->image->depth, window->image->format, 0,
        window->shm_seg, 0
    );

    xcb_flush(window->connection);
    return true;
}

void platform_free() {
    // Only loop over the number of windows actually created
    for (size_t i = 0; i < window_configs_count; i++) {
        X11Window* window = window_configs[i];
        if (window == NULL) continue;

        if (window->connection)
            xcb_disconnect(window->connection);

        if (window->config.framebuffer != NULL) {
            free(window->config.framebuffer);
            window->config.framebuffer = NULL; // prevent accidental reuse
        }

        if (window->config.world_handler != NULL) {
            world_handler_free(window->config.world_handler);
            window->config.world_handler = NULL;
        }

        if (window->screen != NULL) {
            free(window->screen);
        }
        
        xcb_shm_detach(window->connection, window->shm_seg);
        shmdt(window->image->data);
        shmctl(window->shm_id, IPC_RMID, 0);
        xcb_image_destroy(window->image);

        free(window);
        window_configs[i] = NULL;
    }

    // Reset the window count
    window_configs_count = 0;
}

#endif
