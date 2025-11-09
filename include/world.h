#ifndef WORLD_H
#define WORLD_H

#include "engine_options.h"
#include "game_object.h"

#define WORLD_OBJECT_BUFFER_SIZE EO_GAME_OBJECT_BUFFER
DECLARE_VECTOR(GameObjectVector, vector_game_object, struct GameObject);


/**
 * @struct Interval
 * @brief An interval, generally used for Buffers and Pools.
 * @var start Inclusive
 * @var end Exclusive
 * Layout: [size_t, size_t]
 * Size: 16, 8
*/
typedef struct Interval {
	size_t start;
	size_t end;
} Interval;


/**
 * @struct WorldConfig
 * @brief Settings and Information on a World and it's data.
 * 
 * @var WorldConfig::buffer_interval
 * Interval of Active Game Objects on the Buffer. Objects outside of this Interval are not considered for Physics Updates, or Rendering.
 * 
 * @var WorldConfig::pool_interval
 * Interval of Active Game Objects on the Pool. Objects outside of this Interval are not considered for Physics Updates, or Rendering.
 *
 * @var WorldConfig::pool_size
 * Starting size of the WorldHandler's Pool when this Config is Loaded. Will not realloc unless necessary.
 * @note this value does not update to changes in the WorldHandler. @see world_handler_get_pool_size
 *
 * @var WorldConfig::reallocation_ratio
 * Ratio to use when automatically reallocating the Object Pool. This is only used when attempting to overflow the Pool.
 *
 * @var WorldConfig::pixels_per_meter
 * Pixels / Meter conversion, this is used in Pose math. @see struct Pose
 *
 * @var WorldConfig::gravity_field
 * The applied Gravity on Dynamic GameObjects. In units of Newtons/Kg || Meters per Second/Second
*/
typedef struct WorldConfig {
	struct Interval buffer_interval;
	struct Interval pool_interval;
	size_t pool_size;
  float reallocation_ratio;
  float gravity_field;
	int pixels_per_meter;
} WorldConfig;


/**
 * TODO: Update Doxys for this whole damn file
 * @struct World
 * @brief Container for a 'World', holds functions and a WorldConfig used by
 * the world.
 *
 * @note For adding GameObjects to the Buffer and Pool @see world
 *
 * @var World::config
 * The WorldConfig used by the World. This will be set to the Active 
 * WorldConfig when this World is set as the Active World in world.
 * @see world_set_active
 *
 * @var World::init
 * Function Pointer to the World's initialization function. This function is
 * called the moment the World is loaded using @see world_load.
 * This should be used to place GameObjects on the Buffer and Pool, as the
 * space has been delegated for the World when this function is called.
 * @see worldconfig_get_buffer_interval
 * @see worldconfig_get_pool_interval
 *
 * @var World::start
 * Function Pointer to the World's start function. This function is called
 * when the World is set as the Active World in the world.
 * This should be used to set the Starting States of concurrent GameObjects
 * or finish up any buisness in init.
 * @see world_set_active
 *
 * @var World::loop
 * Function Pointer to the World's loop function. This function is called
 * every Fixed Update, when the World is set as the Active World in world.
 * @see world_set_active
 *
 * @var World::close
 * Function Pointer to the World's close function. This function is called
 * when this World was the active one, until the active world changed in
 * world.
 * @see world_set_active
 *
 * @var World::dealloc
 *
 *
*/
typedef struct World {
	struct WorldConfig config;
	struct GameObject object_buffer[WORLD_OBJECT_BUFFER_SIZE];
	struct GameObjectVector object_pool;
	struct Interval interval_buffer;
	struct Interval interval_pool;
	void (*init)();
	void (*start)();
	void (*loop)();
	void (*close)();
	void (*dealloc)();
} World;


/// @brief Initializes a new World with default parameters.
/// @param world To be initialized.
/// @param config WorldConfig to initialize with. Deepcopied.
bool world_new(struct World* world, struct WorldConfig config);

/// @brief Reallocate the Object Pool.
/// @param newSize
/// @return Success of Reallocation.
/// @warning
bool world_realloc_pool(struct World* world, size_t newSize);

/// @brief Nullifies Objects within the Interval of the Object Buffer.
/// @param interval
/// @return Success of the operation.
bool world_remove_from_buffer(struct World* world, struct Interval* interval);

/// @brief Nullifies Objects within the Interval of the Object Pool.
/// @param interval
/// @return Success of the operation.
bool world_remove_from_pool(struct World* world, struct Interval* interval);

/// @brief Returns the Pointer to the GameObject on the Buffer at that Index.
/// @param index on buffer.
struct GameObject* world_buffer_get_object(struct World* world, size_t index);

/// @brief Returns the Pointer to the GameObject on the Pool at that Index.
/// @param index in pool.
struct GameObject* world_pool_get_object(struct World* world, size_t index);

#endif // WORLD_H
