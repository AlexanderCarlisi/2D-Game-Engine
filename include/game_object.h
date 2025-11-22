//
// GameObjects are the parts of the game.
// They have a Position, Size, Color, CollisionBox
// <p>
// I might want to seperate a Physical Position/Size and a Rendering Position/Size
// This would allow for Collisions that don't represent the look of an Object, it would be more versatile.
// <p>
// These Objects will also need some actual Physics: Acceleration, Velocity, Mass, Fiction(to calc coefficient)
//

//
// Two types of objects
// STATIC : Not affected by Physics, is still drawn every frame. Using functions to update Position will still work.
//  *** While static objects arent AFFECTED by physics, they will still affect other objects, unless stated otherwise.
// DYNAMIC : Affected by Physics, gravity and collisions. Ofcourse you can change these behaviors.
//

// TODO: Fix name formatting in Structs
// TODO: Proper Rotation.

#ifndef GAME_OBJECT_H
#define GAME_OBJECT_H

#include "collision_box.h"


// Dynamic Array for Collision Boxes
DECLARE_VECTOR(CollisionBoxVector, vector_collision_box, struct CollisionBox)


// Layout: [0, 1]
// Size: 1 byte
typedef enum ObjectType {
    STATIC, // Not affected by Forces.
    DYNAMIC // Affected by Forces.
} ObjectType;


// Layout: [Vector, Pose, Pose, int, float, 1bEnum]
// Size: 73(80), 57(60)
typedef struct GameObject {
    struct CollisionBoxVector collider_vector;
    struct Pose pose; // Bottom Left relative to CollionBox
    struct Pose previous_pose;
    int world_index;
    float rotation_degrees;
    enum ObjectType object_type;
} GameObject;


// Layout: [Vector, Pose, float, 1bEnum]
// Size: 53(56), 37(40)
typedef struct GameObjectConfig {
    struct Pose starting_pose;
    size_t num_colliders;       // Amount of colliders to pre-alloc for
    float starting_rotation;
    enum ObjectType object_type;
} GameObjectConfig;


/// @brief Initializes the GameObject with the provided Config.
void gameobject_init(struct GameObject* obj, struct GameObjectConfig* config);

/// @brief Add a collision box to the collider_vector. Deepcopy.
void gameobject_add_collider(struct GameObject* obj, CollisionBox cb);

/// @brief Prints debug information about a GameObject.
void gameobject_health_check(struct GameObject* obj);

#endif // GAME_OBJECT_H
