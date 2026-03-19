#ifndef CONTROLLER_H

#include "game_object.h"

typedef struct ObjController {
    GameObject* obj;

    int upKeyCode;
    int downKeyCode;
    int leftKeyCode;
    int rightKeyCode;
    int cwKeyCode;
    int ccwKeyCode;

    float mps; // Speed (m/s)
    float omg; // Angular Speed (deg/s)
} ObjController;

void objcontroller_update(ObjController* controller, pixel_t PPM);

#endif // CONTROLLER_H