#include "MyGame.h"
#include <stdio.h>

#include <platform.h>
#include "color.h"

/// You should know the index you want each world to represent
/// Since it wont change :)
#define WORLD_OVERWORLD   0
#define WORLD_NETHER      1
#define WORLD_END         2
#define PPM               20 // probably a nice idea

WorldConfig overworld_config = {
  .buffer_interval = (Interval) {.start = 0, .end = 0},
  .pool_interval = (Interval) {.start = 0, .end = 0},
  .pool_size = 10,
  .gravity_field = 9.8,
  .pixels_per_meter = PPM,
  .reallocation_ratio = 1.5
};

WorldConfig neth_end_config = {
  .buffer_interval = (Interval) {.start = 0, .end = 0},
  .pool_interval = (Interval) {.start = 0, .end = 0},
  .pool_size = 50,
  .gravity_field = 5.3,
  .pixels_per_meter = PPM,
  .reallocation_ratio = 1.5
};

void overworld_init() { printf("\n>>> OW_INIT <<<\n"); }
void overworld_start() { printf("\n>>> OW_START <<<\n"); }
// void overworld_loop() { printf("\n>>> OW_LOOP <<<\n"); }
void overworld_loop() { }
void overworld_close() { printf("\n>>> OW_CLOSE <<<\n"); }
void overworld_dealloc() { printf("\n>>> OW_DEALLOC <<<\n"); }


static GameObjectConfig goc = (GameObjectConfig) {
  .num_colliders = 1,
  .object_type = DYNAMIC,
  .starting_pose = {0},
  .starting_rotation = 0
};
void load_player(GameObject* obj) {
  gameobject_init(obj, &goc);
  gameobject_add_collider(obj, collision_box_create_debug(
    rgba(255, 0, 0, 255),
    rgba(255, 0, 0, 255),
    pose_from_meters(PPM, 25, 25)
  ));
}


void game_init() {
  printf("Hello World!");

  WINDOWINFO* window = platform_new_window(
    "MyGame",
    (Aspect) {.width=1280, .height=720},
    (Aspect) {.width=1280, .height=720},
    1.0/60
  );

  if (window == NULL) {
    printf("\n>>> Window is NULL <<<\n");
    return;
  }
  
  // You should instantiate your worlds asap.
  // They wont actually be 'loaded' until you explictily call it to be.
  // To 'load' a world, you will implement what that means in a world's init function.
  window->config.world_handler = world_handler_new();
  
  if (window->config.world_handler == NULL) {
    printf("\n>>> Handler is NULL <<<\n");
    return;
  }

  WorldHandler* whand = window->config.world_handler;

  world_handler_new_world(whand, overworld_config);
  world_handler_new_world(whand, neth_end_config); // configs are deepcopied
  world_handler_new_world(whand, neth_end_config); // so they can be changed independantly later
  
  // Store pointers to worlds if you want
  World* overworld = &whand->worlds.data[WORLD_OVERWORLD];

  // Setup how worlds will operate
  // These functions are called automatically by the WorldHandler
  overworld->init = overworld_init;       // 'load' the world, allocate memory
  overworld->start = overworld_start;     // world is now active, do this
  overworld->loop = overworld_loop;       // Called every frame
  overworld->close = overworld_close;     // world is no longer active, do this
  overworld->dealloc = overworld_dealloc; // deallocate ALL dynamic memory you owm
  
  // You have two main allocations of Memory for GameObjects in a World.
  // The Buffer, and the Pool.
  // The Buffer is a statically allocated array of GameObjects.
  // The Pool is a Dynamically allocated array of GameObjects.
  // 
  // Why use intervals?
  // Its more memory efficient than wasting a byte to check if each object is active or not
  // additionally its more time efficient than checking if all 10k of your objects are active.
  // also you should be working with your memory all nice and bundled together because of how
  // the CPU cache actually caches through the cache line.
  // Lots more reason to have it be this way, then it be the other way.
  Interval bufferInterval = (Interval) {.start=0, .end=1};
  Interval poolInterval = (Interval) {.start = 0, .end=1};

  // Generally in the Buffer, you want to keep Object locations consitent, as
  // the purpose of the Buffer is to be persistent memory.
  const int PLAYER_SPRITE = 0; // Index in buffer representing the player sprite
  if (overworld == NULL) {
    printf("\n>>> overworld is NULL <<<\n");
    return;
  }
  GameObject* player = world_buffer_get_object(overworld, PLAYER_SPRITE);
  if (player == NULL) {
    printf("\n>>> player is NULL <<<\n");
    return;
  }
  
  // Stay clean, its C, the safer I make it, the more verbose it gets
  load_player(player);
  pose_update_meters(&player->pose, PPM, 10, 10);

  int floor_sprite = 0;
  GameObject* floor = world_pool_get_object(overworld, floor_sprite);
  gameobject_init(floor, &goc);
  
  // Set the invervals on the World, so it knows whats active.
  // Active objects get rendered and have physics updates
  // You an have a Object declared as STATIC if you want NO physics updates.
  // You can also not give Objects 0 colliders. NOT NULL, 0
  overworld->interval_buffer = bufferInterval;
  overworld->interval_pool = poolInterval;
  
  // Set worlds as active
  overworld->init(); // LOAD THE WORLD, YOU CALL THIS
  world_handler_set_active(whand, overworld); // Calls START
  // Now the world will be handled by the Handler.
  
  // So, how does the engine work its magic?
  // Done. All Windows will be iterated through and handled.
  // All WorldHandlers tied to windows will be iterated through and handled.
  // All ACTIVE worlds tied to WorldHandlers will iterated through and handled.
  platform_initialize();

  // Additional Notes:
  // Want Windows to use the same Worlds? WorldHandler is a Pointer for that very reason!
}

void game_loop() {
  // Loop is called every fixed update. @see engine_tick
  // Its called after Inputs are handled from the platform
  // called before things are rendered.
  //
  // Handle additional logic here, but keep in mind, things WILL get messy.
  // Worlds have a Loop function, for organization.
  // This gets called after platform_iterate, in engine.c
  // printf("Loop!");
}

void game_free() {
  // Deallocate any Resources not managed by the Engine
  // What resources are managed by the Engine? Basically anything u got from the engine.
  // TODO: Better docs on what is and isnt managed by the engine
}
