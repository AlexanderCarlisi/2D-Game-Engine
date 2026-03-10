#ifndef POSE_H
#define POSE_H

#include "stdbool.h"
#include "stdint.h"

struct World; // WORLD_H
typedef int16_t pixel_t;
typedef float meter_t;

/// @struct Pose
/// @brief A position in the World.
///
/// @var Pose::x_meters
/// Horizontal Position, in meters.
///
/// @var Pose::y_meters
/// Verticle Position, in meters.
///
/// @var Pose::x_pixels
/// Horizontal Position, in pixels.
///
/// @var Pose::y_pixels
/// Verticle Position, in pixels.
/// 
/// @note Pose values should be updated using the related functions.
/// @see pose_update_meters
/// @see pose_update_pixels
///
/// @note Your game should be manipulating Meter values, updating Pixel values
/// will get messy, as the pixel values here are used directly by the Renderer.
///	
///	@note Meters and Pixel updates are proportional to each other based on the
///	WorldConfig. @file world_config.h
///	
/// @warning Updating one of these values on your own (outside the provided functions)
/// will cause problems if you aren't updating the Meters and Pixels values.
/// 
/// Layout: [float, float, ui16, ui16]
/// Size: 12(18), 12 bytes
typedef struct Pose {
    meter_t x_meters;
    meter_t y_meters;
    pixel_t x_pixels;
    pixel_t y_pixels;
} Pose;

/// @brief Generate a new Pose struct from given Pixels.
Pose pose_from_pixels(pixel_t ppm, pixel_t x, pixel_t y);

/// @brief Generate a new Pose struct from given Meters.
Pose pose_from_meters(pixel_t ppm, meter_t x, meter_t y);

/// @brief Updates the Pose on new Meter coordinates.
/// @param pose Pose Pointer to update.
/// @param ppm Pixels Per Meters
/// @param x in meters
/// @param y in meters
void pose_update_meters(struct Pose* pose, pixel_t ppm, meter_t x, meter_t y);

/// @brief Updates the Pose on new Pixel coordinates.
/// @param pose Pose Pointer to update.
/// @param ppm Pixels Per Meters
/// @param x in pixels
/// @param y in pixels
/// @warning It's suggested to stick with the Meters system, instead of manipulating
/// pixels directly.
void pose_update_pixels(struct Pose* pose, pixel_t ppm, pixel_t xPixels, pixel_t yPixels);

/// @brief Checks if two poses are equal in Meter Positions.
/// @param pose1
/// @param pose2
/// @return True | False
bool pose_equals(struct Pose* pose1, struct Pose* pose2);

/// @brief Checks if the pixels of pose and within the bounds of xUpper and yUpper.
/// @param pose Pose to check with
/// @param xUpper upper boundary in pixels (exclusive)
/// @param yUpper upper boundary in pixels (exclusive)
/// @return true if the pose is valid, false otherwise
/// @note lower bound: [(0,0),(xUpper,yUpper))
bool pose_pixels_in_bounds(struct Pose* pose, pixel_t xUpper, pixel_t yUpper);

#endif // POSE_H
