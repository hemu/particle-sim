#pragma once
#include "cglm/cglm.h"

typedef struct Line {
    vec3 start;
    vec3 end;
    int VAO;
} Line;

Line* NewLine(vec3 start, vec3 end);