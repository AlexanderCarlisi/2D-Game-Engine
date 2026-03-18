#include "pose.h"
#include <stddef.h>

void pose_update_meters(struct Pose* pose, pixel_t ppm, meter_t x, meter_t y) {
	if (pose == NULL) return;
	pose->x_meters = x;
	pose->y_meters = y;
	pose->x_pixels = x * ppm;
	pose->y_pixels = y * ppm;
}

void pose_update_pixels(struct Pose* pose, pixel_t ppm, pixel_t x, pixel_t y) {
	if (pose == NULL) return;
	pose->x_meters = (float) x / ppm;
	pose->y_meters = (float) y / ppm;
	pose->x_pixels = x;
	pose->y_pixels = y;
}

bool pose_equals(struct Pose* pose1, struct Pose* pose2) {
	return pose1->x_meters == pose2->x_meters && pose1->y_meters == pose2->y_meters;
}

bool pose_pixels_in_bounds(struct Pose* pose, pixel_t xUpper, pixel_t yUpper) {
	if (pose == NULL) return false;
	return pose->x_pixels >= 0 && pose->y_pixels >= 0 && pose->x_pixels < xUpper && pose->y_pixels < yUpper;
}

struct Pose pose_from_pixels(pixel_t ppm, pixel_t x, pixel_t y) {
	struct Pose p;
	pose_update_pixels(&p, ppm, x, y);
	return p;
}

struct Pose pose_from_meters(pixel_t ppm, meter_t x, meter_t y) {
	struct Pose p;
	pose_update_meters(&p, ppm, x, y);
	return p;
}
