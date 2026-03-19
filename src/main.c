// WIN: gcc src/*.c example/src/*.c -Iinclude -Iexample/include -o 2DGameEngine -lgdi32
// LX: gcc src/*.c example/src/*.c -Iinclude -Iexample/include -o 2DGameEngine -lxcb -lxcb-shm -lm

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
// TODO: Compile Example and Engine seperately
//
// Currently Implementing:
//  Fix FB Issues
//  Windows Refactor

#include "engine_options.h"
#include "logger.h"
#include "stdio.h"
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
    printf("Include a 1 or 0 you fucking idiot");
    return 0;
  }
  EO_PROJECT_MAIN_START;
  return 0;
}