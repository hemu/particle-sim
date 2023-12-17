#pragma once
#include <cglm/cglm.h>

extern vec3 UNIT_UP;
extern vec3 UNIT_DOWN;
extern vec3 UNIT_LEFT;
extern vec3 UNIT_RIGHT;

typedef enum Dir {
    DIR_NONE,
    DIR_LEFT,
    DIR_RIGHT,
    DIR_BOT,
    DIR_TOP
} Dir;

