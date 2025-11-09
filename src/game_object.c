#include "game_object.h"
#include "shape.h"
#include <stdio.h>

DEFINE_VECTOR(CollisionBoxVector, vector_collision_box, struct CollisionBox)


// TODO: [?] If this becomes a reoccuring pattern, add it to vector.h
CollisionBoxVector create_empty_collision_box_vector(size_t size) {
    CollisionBoxVector cbv;
    vector_collision_box_init(&cbv, size);
    return cbv;
}


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
    obj->collider_vector = config->collider_vector;
}
