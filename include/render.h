#ifndef RENDER_H
#define RENDER_H

#include "platform.h"
#include "pose.h"

void render_init(struct WindowConfig* window);
void render_clear(struct WindowConfig* window, uint32_t color);
void render_draw_pixel(struct WindowConfig* window, Pose* point, uint32_t color);
void render_interpolate_pixel(struct WindowConfig* window, struct Pose* point1, struct Pose* point2, uint32_t color, float alpha);
void render_draw_shape(struct WindowConfig* window, struct Shape* shape, uint32_t color, int vertices);
void render_draw(struct WindowConfig* window, float alpha);

// TODO: More OS Specific
// void render_display(struct WindowConfig* window);
// void render_free(struct WindowConfig* window);

#endif
