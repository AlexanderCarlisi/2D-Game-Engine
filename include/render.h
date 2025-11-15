#ifndef RENDER_H
#define RENDER_H

#include "platform.h"
#include "pose.h"

void render_init(struct WindowConfig* config);
void render_clear(struct WindowConfig* config, uint32_t color);
void render_draw_pixel(struct WindowConfig* config, Pose* point, uint32_t color);
void render_interpolate_pixel(struct WindowConfig* config, struct Pose* point1, struct Pose* point2, uint32_t color, float alpha);
void render_draw_shape(struct WindowConfig* config, struct Shape* shape, uint32_t color);
void render_draw(struct WindowConfig* config, float alpha);

#endif
