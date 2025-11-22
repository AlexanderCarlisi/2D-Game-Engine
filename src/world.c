#include "world.h"
#include <string.h>

DEFINE_VECTOR(GameObjectVector, vector_game_object, struct GameObject);


bool world_realloc_pool(struct World* world, size_t newSize) {
	if (world->object_pool.data == NULL) {
		return vector_game_object_init(&world->object_pool, newSize);

	} else if (world->object_pool.size == newSize) {
		return vector_game_object_realloc(&world->object_pool);

	} else {
		return vector_game_object_realloc_set(&world->object_pool, newSize);
	}
}

bool world_remove_from_buffer(struct World* world, struct Interval* interval) {
	if (interval->end >= WORLD_OBJECT_BUFFER_SIZE) {
		return false;
	}
	for (size_t i = interval->start; i < interval->end; i++) {
	 	struct GameObject* go = &world->object_buffer[i];
		vector_collision_box_free(&go->collider_vector);
		memset(go, 0, sizeof(struct GameObject));
	}
	return true;
}

bool world_remove_from_pool(struct World* world, struct Interval* interval) {
	if (interval->end >= WORLD_OBJECT_BUFFER_SIZE) {
		return false;
	}
	for (size_t i = interval->start; i < interval->end; i++) {
	  struct GameObject* go = &world->object_pool.data[i];
	  vector_collision_box_free(&go->collider_vector);
		memset(go, 0, sizeof(struct GameObject));
	}
	return true;
}

struct GameObject* world_buffer_get_object(struct World* world, size_t index) {
	if (index >= WORLD_OBJECT_BUFFER_SIZE) return NULL;
	return &world->object_buffer[index];
}

struct GameObject* world_pool_get_object(struct World* world, size_t index) {
	if (index >= WORLD_OBJECT_BUFFER_SIZE) return NULL;
	return &world->object_pool.data[index];
}

bool world_new(struct World* world, struct WorldConfig config) {
	if (world == NULL) return false;
	
	world->interval_buffer = config.buffer_interval;
	world->interval_pool = config.pool_interval;
	world->object_pool.realloc_ratio = config.reallocation_ratio;
	return world_realloc_pool(world, config.pool_size);
}
