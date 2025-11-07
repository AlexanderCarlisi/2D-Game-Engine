#include "pose.h"

void pose_update_meters(struct Pose* pose, int pixelsPerMeter, float xMeters, float yMeters) {
	pose->x_meters = xMeters;
	pose->y_meters = yMeters;
	pose->x_pixels = xMeters * pixelsPerMeter;
	pose->y_pixels = yMeters * pixelsPerMeter;
}

void pose_update_pixels(struct Pose* pose, int pixelsPerMeters, float xPixels, float yPixels) {
	pose->x_meters = xPixels / pixelsPerMeters;
	pose->y_meters = yPixels / pixelsPerMeters;
	pose->x_pixels = xPixels;
	pose->y_pixels = yPixels;
}

bool pose_equals(struct Pose* pose1, struct Pose* pose2) {
	return pose1->x_meters == pose2->x_meters && pose1->y_meters == pose2->y_meters;
}
