#include "engine.h"
#include "engine_options.h"
#include "time.h"
#include EO_PROJECT_MAIN

static bool is_running = false;

void engine_start() {
    is_running = true;
}

bool engine_tick(struct WindowConfig* window) {
    float currentTime = get_current_time();
    float frameTime = currentTime - window->previous_time;
    float fixedTimeStep = window->frames_per_second;
    window->previous_time = currentTime;
    window->accumulator += frameTime;

    // Fixed Update
    while (window->accumulator >= fixedTimeStep) {
        // Update Game Logic
        platform_iterate(window);
        EO_PROJECT_MAIN_LOOP;
        window->accumulator -= fixedTimeStep;
    }

    // Variable Update
    float alpha = window->accumulator / fixedTimeStep;
    platform_render(window, alpha);
    
    return false;
}

void engine_close() {
    platform_free();
}

void engine_set_running(bool state) {
    is_running = state;
}

bool engine_is_running() {
    return is_running;
}
