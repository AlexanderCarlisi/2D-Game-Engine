#ifndef MY_GAME_H

/// @brief  Initialize whatever you need for you game.
/// Called in main.c after initializing the Logger
void game_init();

/// @brief The Main loop for your game.
/// Called in engine.c's Fixed Update
/// @note Since this is in a Fixed Update, it's not optimal to mess with any
///     rendering here, as it will not be flushed until the update is over.
void game_loop();

/// @brief Called on Engine Close, handle anything YOU allocated on the heap,
///     the engine will handle all of the things IT allocated.
/// @note the Logger is deallocated AFTER this is called, so feel free to add
///     logging to this function.
void game_free();

#endif // MY_GAME_H