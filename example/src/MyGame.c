#include "MyGame.h"
#include "platform.h"
#include "overworld.h"

void game_init() {
    printf("Hello World!");

    // Creates a Window in platform.h, handled all in the Engine
    AWINDOW* window = platform_new_window(
        "MyGame",
        (Aspect) {.width=1280, .height=720},
        (Aspect) {.width=1280, .height=720},
        60
    );
    if (window == NULL) return;
    // Keeping a pointer to the window allows you to make
    // changes to it later

    // Create a World Handler, to deal with the Worlds.
    window->config.world_handler = world_handler_new();
    if (window->config.world_handler == NULL) return;

    WorldHandler* whand = window->config.world_handler;
    overworld_setup_handle(whand);

    // Load World (allocate necessary resources, and whatever you want)
    overworld_init();
    world_handler_set_active(whand, &whand->worlds.data[OVERWORLD_HANDLER_INDEX]);

    // Start the Engine
    platform_initialize();
}

void game_loop() {

}

void game_free() {
    overworld_close();
    overworld_free();
}