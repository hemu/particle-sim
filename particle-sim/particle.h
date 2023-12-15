#pragma once
#include <cglm/cglm.h>

typedef struct {
    vec3 pos;
    vec3 pos_prev;
    vec3 vel;
    int VAO;
} Particle;

Particle* ParticleNew();
