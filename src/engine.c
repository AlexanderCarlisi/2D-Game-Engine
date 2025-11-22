#include "engine.h"
#include "engine_options.h"
#include "e_time.h"
#include "logger.h"
#include EO_PROJECT_MAIN

static bool is_running = false;

void engine_start() {
    is_running = true;
    logger_write(1, 0, "Engine Started", false);
}

bool engine_tick(struct WINDOWINFO* window) {
    if (window->config.previous_time == 0)
        window->config.previous_time = etime_ms();
    uint64_t currentTime = etime_ms();
    uint64_t frameTime = currentTime - window->config.previous_time;
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
    logger_free();
}

void engine_set_running(bool state) {
    is_running = state;
}

bool engine_is_running() {
    return is_running;
}
