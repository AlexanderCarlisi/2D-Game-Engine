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
	uint pixels_per_meter;
} WorldConfig;


/**
 * TODO: Remove WorldConfig from World. It's unnecessary
 * @struct World
 *
 * <p>
 * A World is a collection of Game Object. Split into a Statically allocated
 * 	buffer, and dynamically allocated pool.
 *
 * <p>
 * There are intervals for the respective Game Object collections denoting 
 * 	what indices are active. Active game object will have physics updates
 * 	and be rendered.
 *
 * <p>
 * There are 5 functions pointers for you to implement.
 * <p> init : Preload any dynamic memory or light/heavy data, the intention
 * 	behind this function is just a suggestion for loading things as the game
 * 	is played, its not a strict requirement. You are responsible for calling
 * 	this function, if ever.
 *
 * <p> start : This function is called whenever the World is set as active in
 * 	the WorldHandler, @see world_handler.h
 *
 * <p> loop : This function is called automatically every loop when the world
 * 	is active.
 *
 * <p> close : This function is caleld automatically when you set anther world
 * 	as active.
 *
 * <p> Dealloc : This function is called automatically on clean exit. You can
 * 	call this function yourself, just make sure to remove the world from the 
 * 	world handler after.
 *
 * @warning object_pool is deallocated automatically for you on clean exit.
 * 	This is not the case if you call dealloc yourself. Make sure to NULL check
 * 	the object_pool.data member, and use the appropriate @see vector.h function
 * 	for dellocating its resources.
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
