#pragma once
#include <cglm/cglm.h>

#include "const.h"
#include "particle.h"
#include "types.h"
#include <math.h>

void HandleCollision(Particle *p, vec3 *next_pos, Dir collide_dir);
