#include "controller.h"
#include "input.h"
#include "stdio.h"

void objcontroller_update(ObjController* controller, pixel_t PPM) {
    // printf("start input\n");
    Pose* curr = &controller->obj->pose;
    meter_t transSpeed = controller->mps / 60; // fps
    float rotSpeed = controller->omg / 60;

    if (input_kb_pressed(controller->upKeyCode)) {
        pose_update_meters(&controller->obj->pose, PPM, curr->x_meters, curr->y_meters - transSpeed);
    }

    if (input_kb_pressed(controller->downKeyCode)) {
        pose_update_meters(&controller->obj->pose, PPM, curr->x_meters, curr->y_meters + transSpeed);
    }

    if (input_kb_pressed(controller->leftKeyCode)) {
        pose_update_meters(&controller->obj->pose, PPM, curr->x_meters - transSpeed, curr->y_meters);
    }

    if (input_kb_pressed(controller->rightKeyCode)) {
        pose_update_meters(&controller->obj->pose, PPM, curr->x_meters + transSpeed, curr->y_meters);
    }

    if (input_kb_pressed(controller->cwKeyCode)) {
        controller->obj->rotation_degrees += rotSpeed;
    }

    if (input_kb_pressed(controller->ccwKeyCode)) {
        controller->obj->rotation_degrees -= rotSpeed;
    }
    // printf("end input\n");
}