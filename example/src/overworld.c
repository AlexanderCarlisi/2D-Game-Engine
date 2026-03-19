#include "overworld.h"
#include "controller.h"
#include "input.h"

#define PLAYER1_BUFFER      0
#define PLAYER2_BUFFER      1
#define FLOATER_BUFFER      2
#define PPM                 10 // Pixels per Meter

WorldConfig WORLD_CONFIG = {
    // Intervals are [start, end)

    // Buffer and Pool are Arrays of Game Objects
    // Buffer is a static Array
    // Pool is a Vector

    // Their respective intervals dictate what objects are 'Active'
    // Active Game Objects are Updated and Rendered.

    .buffer_interval = (Interval) {.start = 0, .end = 3},
    .pool_interval = (Interval) {.start = 0, .end = 0},
    .pool_size = 0,
    .gravity_field = 9.8,       // Physics, not implemented
    .pixels_per_meter = PPM,    // Objects have Poses, poses are both in Meters and Pixels
    .reallocation_ratio = 1.5   // Pool Reallocation factor if overflowing
};

GameObjectConfig OBJECT_CONFIG = {
    // Currently Collision Boxes are what define Objects, since there are no Textures
    .num_colliders = 1,         // Number of Collision Boxes
    .object_type = DYNAMIC,     // Dynamic (Physics, Collisions), Static (none of that)

    // These fields don't really matter, you can set in init easier
    // (cant call functions in constant definitions, so it's a lil annoying)
    .starting_pose = {0},       // Top Left (0, 0)
    .starting_rotation = 0      // In degrees
};

static World* overworld_ptr = NULL;
static GameObject* player1_ptr = NULL;
static GameObject* player2_ptr = NULL;
static GameObject* floater_ptr = NULL;
static ObjController player1_ctrl;
static ObjController player2_ctrl;

void load_player1();
void load_player2();
void load_floater();

void overworld_setup_handle(WorldHandler* handler) {
    // Many functions return a boolean value for success and failure, be sure to check everything
    if(!world_handler_new_world(handler, WORLD_CONFIG)) {
        logger_write(2, 1, "overworld_setup_handle: Failed to setup world", true);
        return;
    }
    overworld_ptr = &handler->worlds.data[OVERWORLD_HANDLER_INDEX];
    overworld_ptr->init = overworld_init;
    overworld_ptr->start = overworld_start;
    overworld_ptr->loop = overworld_loop;
    overworld_ptr->close = overworld_close;
    overworld_ptr->dealloc = overworld_free;
}

WorldConfig overworld_config() {
    return WORLD_CONFIG;
}

void overworld_init() {
    logger_write(2, 1, "Overworld INIT!", false);

    // Obtain references to GameObjects
    player1_ptr = world_buffer_get_object(overworld_ptr, PLAYER1_BUFFER);
    player2_ptr = world_buffer_get_object(overworld_ptr, PLAYER2_BUFFER);
    floater_ptr = world_buffer_get_object(overworld_ptr, FLOATER_BUFFER);

    // Load Objects
    load_player1();
    load_player2();
    load_floater();
}

void overworld_start() {
    logger_write(2, 1, "Overworld START!", false);

    // Starting Positions
    pose_update_pixels(&player1_ptr->pose, PPM, 1280 / 2, 720 / 2);
    pose_update_pixels(&player2_ptr->pose, PPM, 0, 0);
    pose_update_pixels(&floater_ptr->pose, PPM, 720/2, 1280*0.75);
}

void overworld_loop() {
    objcontroller_update(&player1_ctrl, PPM);
    objcontroller_update(&player2_ctrl, PPM);
}

void overworld_close() { }

void overworld_free() { }

void load_player1() {
    gameobject_init(player1_ptr, &OBJECT_CONFIG);
    
    Shape shape = {
        .size = pose_from_meters(PPM, 5, 5),
        .color = rgba(255, 0, 0, 255)
    };
    shape_init(&shape, 5);
    shape_add_convex_vertices(&shape, 5);
    player1_ptr->rotation_degrees = 0;
    
    gameobject_add_collider(
        player1_ptr, 
        (CollisionBox) {
            .shape = shape,
            .on_collision_enter = 0,
            .on_collision_exit = 0,
            .debug_color = rgba(0, 255, 0, 255)
        }
    );
    
    // Unexpected Behavior? Debug it
    gameobject_health_check(player1_ptr); // Logs current state of gameobject (probably shouldn't put this in a loop)

    player1_ctrl.obj = player1_ptr;
    player1_ctrl.upKeyCode = INPUT_KEYB_W;
    player1_ctrl.downKeyCode = INPUT_KEYB_S;
    player1_ctrl.leftKeyCode = INPUT_KEYB_A;
    player1_ctrl.rightKeyCode = INPUT_KEYB_D;
    player1_ctrl.cwKeyCode = INPUT_KEYB_E;
    player1_ctrl.ccwKeyCode = INPUT_KEYB_Q;
    player1_ctrl.mps = 50;
    player1_ctrl.omg = 30;
}

void load_player2() {
    gameobject_init(player2_ptr, &OBJECT_CONFIG);
    
    Shape shape = {
        .size = pose_from_meters(PPM, 3, 5),
        .color = rgba(0, 255, 0, 255)
    };
    shape_init(&shape, 100);
    shape_add_convex_vertices(&shape, 100);
    player2_ptr->rotation_degrees = 0;
    
    gameobject_add_collider(
        player2_ptr, 
        (CollisionBox) {
            .shape = shape,
            .on_collision_enter = 0,
            .on_collision_exit = 0,
            .debug_color = rgba(0, 255, 0, 255)
        }
    );
    
    // Unexpected Behavior? Debug it
    gameobject_health_check(player2_ptr); // Logs current state of gameobject (probably shouldn't put this in a loop)

    player2_ctrl.obj = player2_ptr;
    player2_ctrl.upKeyCode = INPUT_KEYB_I;
    player2_ctrl.downKeyCode = INPUT_KEYB_K;
    player2_ctrl.leftKeyCode = INPUT_KEYB_J;
    player2_ctrl.rightKeyCode = INPUT_KEYB_L;
    player2_ctrl.cwKeyCode = INPUT_KEYB_O;
    player2_ctrl.ccwKeyCode = INPUT_KEYB_U;
    player2_ctrl.mps = 40;
    player2_ctrl.omg = 80;
}

void load_floater() {
    gameobject_init(floater_ptr, &OBJECT_CONFIG);
    
    Shape shape = {
        .size = pose_from_meters(PPM, 10, 3.5f),
        .color = rgba(0, 0, 0, 0)
    };
    shape_init(&shape, 4);
    shape_add_convex_vertices(&shape, 4);
    floater_ptr->rotation_degrees = 0;
    
    gameobject_add_collider(
        floater_ptr, 
        (CollisionBox) {
            .shape = shape,
            .on_collision_enter = 0,
            .on_collision_exit = 0,
            .debug_color = rgba(0, 255, 0, 255)
        }
    );
    
    // Unexpected Behavior? Debug it
    gameobject_health_check(floater_ptr); // Logs current state of gameobject (probably shouldn't put this in a loop)
}