#include "engine.h"
#include "engine_options.h"
#include "time.h"
#include EO_PROJECT_MAIN

static bool is_running = false;

void engine_start() {
    is_running = true;
    printf("\n>>> Engine Started! <<<\n");
}

bool engine_tick(struct WINDOWINFO* window) {
    float currentTime = get_current_time();
    float frameTime = currentTime - window->config.previous_time;
    float fixedTimeStep = window->config.frames_per_second;
    window->config.previous_time = currentTime;
    window->config.accumulator += frameTime;

    // Fixed Update
    while (window->config.accumulator >= fixedTimeStep) {
        // Update Game Logic
        platform_iterate(window);
        EO_PROJECT_MAIN_LOOP;
        window->config.accumulator -= fixedTimeStep;
    }

    // Variable Update
    float alpha = window->config.accumulator / fixedTimeStep;
    platform_render(window, alpha);
    
    return false;
}

void engine_close() {
    platform_free();
    EO_PROJECT_MAIN_FREE;
}

void engine_set_running(bool state) {
    is_running = state;
}

bool engine_is_running() {
    return is_running;
}
