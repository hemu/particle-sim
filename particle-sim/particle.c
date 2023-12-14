#include "particle.h"

Particle ParticleNew() {
    Particle p = { .pos = {0.0, 0.0, 0.0}, .pos_prev = {0.0, 0.0, 0.0}, .vel = {0.0, 0.0, 0.0} };
    return p;
}