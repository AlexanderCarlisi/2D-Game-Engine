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

/// @struct GameObject
///
/// <p> A collection of Collision Boxes with a position and rotation.
///         Can be defined as either STATIC or DYNAMIC @see ObjectType
///
/// <p> Initialized with @see GameObjectConfig
///
/// Layout: [Vector, Pose, Pose, float, 1bEnum]
/// Size: 73(80), 57(60)
typedef struct GameObject {
    struct CollisionBoxVector collider_vector;
    struct Pose pose;                           // Top left, relative to collision box shape normalized vertices
    struct Pose previous_pose;
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
