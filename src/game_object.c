#include "game_object.h"
#include "shape.h"
#include <stdio.h>
#include "logger.h"

DEFINE_VECTOR(CollisionBoxVector, vector_collision_box, struct CollisionBox)

void gameobject_init(struct GameObject* obj, struct GameObjectConfig* config) {
    if (obj == NULL) {
        logger_write(2, 1, "gameobject_init: NULL obj", true);
        return;   
    }

    if (config == NULL) {
        logger_write(2, 1, "gameobject_init: NULL config", true);
        return;
    }
    
    obj->object_type = config->object_type;
    obj->pose = config->starting_pose;
    obj->previous_pose = config->starting_pose;
    obj->rotation_degrees = config->starting_rotation;
    
    vector_collision_box_init(&obj->collider_vector, config->num_colliders);
}

void gameobject_add_collider(struct GameObject* obj, CollisionBox cb) {
    if (obj == NULL) return;
    vector_collision_box_add(&obj->collider_vector, cb);
}

void gameobject_health_check(struct GameObject* obj) {
    if (obj == NULL) {
        logger_write(2, 2, "gameobject_health: obj null", true);
        return;
    }
    if (obj->collider_vector.size == 0) {
        logger_write(2, 2, "gameobject_health: collider vector uninitialized", true);
        return;
    }
    
    for (size_t i = 0; i < obj->collider_vector.count; i++) {
        logger_write(2, 2, "gameobject_health: collision box", false);
        collision_box_health_check(&obj->collider_vector.data[i]);
    }
}

void gameobject_free(struct GameObject* obj) {
    for (size_t c = 0; c < obj->collider_vector.count; c++) {
        struct CollisionBox collisionBox = obj->collider_vector.data[c];
        shape_free(&collisionBox.shape);
    }
}
