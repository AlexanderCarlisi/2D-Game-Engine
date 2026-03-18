// WIN: gcc src/*.c -Iinclude -o 2DGameEngine -lgdi32
// LINUX: gcc src/*.c -Iinclude -o 2DGameEngine -lxcb -lxcb-shm -lm

// TODO: ADDITION: GameObject Textures
//
// TODO: ADDITION: World Physics, Collisions, CollisionBoxes
//
// TODO: EDIT: Logging
//          Can be cleaned up a bit, less verbose
//          Logs directory, filename dates
//
// TODO: Incorperate alpha param into Render algo
//
// Currently Implementing:
//  Fix FB Issues
//  Windows Refactor

#include "engine_options.h"
#include "logger.h"
#include EO_PROJECT_MAIN

/// args[0] = Filename, implicit
/// args[1] = Log to file property
///     '0': Append to file
///     '1': Clear file, Write
int main(int argc, char *argv[]) {
  if (argc > 1) {
    if (argv[1][0] == '0') {
      logger_init(0);
    } else if (argv[1][0] == '1') {
      logger_init(1);
    }
  } else {
    return 0;
  }
  EO_PROJECT_MAIN_START;
  return 0;
}
