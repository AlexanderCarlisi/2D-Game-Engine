#include "collision_box.h"
#include <stdio.h>

void debug_collision_enter_function(struct GameObject* self, struct GameObject* collidedObject) { printf("\n>> Collision Enter <<"); }
void debug_collision_exit_function(struct GameObject* self, struct GameObject* collidedObject) { printf("\n>> Collision Exit <<"); }

CollisionBox collision_box_create_debug(uint32_t debugColor, uint32_t shapeColor, struct Pose shapeSize) {
    CollisionBox collisionBox = {
        .debug_color = debugColor,
        .on_collision_enter = debug_collision_enter_function,
        .on_collision_exit = debug_collision_exit_function,
        .shape = shape_create_box(shapeSize, shapeColor)
    };
    return collisionBox;
}

void collision_box_health_check(struct CollisionBox* cb) {
    // TODO: Add checks for if enter and exit methods are null pointers
    if (cb == NULL) {
        printf("\n<DEBUG>\t CollisionBox NULL Obj");
        return;
    }

    shape_health_check(&cb->shape);
}
