#include "pose.h"
#include <stddef.h>

void pose_update_meters(struct Pose* pose, int pixelsPerMeter, float xMeters, float yMeters) {
	if (pose == NULL) return;
	pose->x_meters = xMeters;
	pose->y_meters = yMeters;
	pose->x_pixels = xMeters * pixelsPerMeter;
	pose->y_pixels = yMeters * pixelsPerMeter;
}

void pose_update_pixels(struct Pose* pose, int pixelsPerMeters, uint16_t xPixels, uint16_t yPixels) {
	if (pose == NULL) return;
	pose->x_meters = (float) xPixels / pixelsPerMeters;
	pose->y_meters = (float) yPixels / pixelsPerMeters;
	pose->x_pixels = xPixels;
	pose->y_pixels = yPixels;
}

bool pose_equals(struct Pose* pose1, struct Pose* pose2) {
	return pose1->x_meters == pose2->x_meters && pose1->y_meters == pose2->y_meters;
}

bool pose_pixels_in_bounds(struct Pose* pose, uint16_t xUpper, uint16_t yUpper) {
	if (pose == NULL) return false;
	return pose->x_pixels <= xUpper && pose->y_pixels <= yUpper;
}

struct Pose pose_from_pixels(int ppm, uint16_t x, uint16_t y) {
	struct Pose p;
	pose_update_pixels(&p, ppm, x, y);
	return p;
}

struct Pose pose_from_meters(int ppm, float x, float y) {
	struct Pose p;
	pose_update_meters(&p, ppm, x, y);
	return p;
}
