#include "game_object.h"
#include "shape.h"
#include <stdio.h>

DEFINE_VECTOR(CollisionBoxVector, vector_collision_box, struct CollisionBox)

void gameobject_init(struct GameObject* obj, struct GameObjectConfig* config) {
    if (obj == NULL) {
        printf("\n>>> gameobject_init NULL OBJ <<<\n");
        return;   
    }

    if (config == NULL) {
        printf("\n>>> gameobject_init config NULL<<<\n");
        return;
    }
    
    obj->object_type = config->object_type;
    obj->pose = config->starting_pose;
    obj->previous_pose = config->starting_pose;
    obj->rotation = config->starting_rotation;
    
    vector_collision_box_init(&obj->collider_vector, config->num_colliders);
}

void gameobject_add_collider(struct GameObject* obj, CollisionBox cb) {
    if (obj == NULL) return;
    vector_collision_box_add(&obj->collider_vector, cb);
}
