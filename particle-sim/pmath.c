#include "pmath.h"

void HandleCollision(Particle *p, vec3 *next_pos, Dir collide_dir) {
    if (collide_dir == DIR_NONE) {
        return false;
    }

    // Calculate collision point by calculating distance to collision surface in one coordinate
    // Use formula for finding side B of right triangle, given side A and adjacent angle:
    // b = a * tan(alpha)
    float angle;

    vec3 unit_vel;
    glm_vec3_normalize_to(p->vel, unit_vel);
    vec3 collision_pos = {0.0f, 0.0f, 0.0f };

    float x_dist = 0.0f;
    float y_dist = 0.0f;

    if (collide_dir == DIR_TOP) {
        angle = glm_vec3_angle(p->pos, UNIT_UP);
        x_dist = y_dist * tan(angle);
        y_dist = BOUND_Y - p->pos[1];
    } else if (collide_dir == DIR_BOT) {
        angle = glm_vec3_angle(p->pos, UNIT_DOWN);
        x_dist = y_dist * tan(angle);
        y_dist = p->pos[1] - (-BOUND_Y);
    } else if (collide_dir == DIR_RIGHT) {
        angle = glm_vec3_angle(p->pos, UNIT_RIGHT);
        x_dist = BOUND_X - p->pos[0];
        y_dist = x_dist * tan(angle);
    } else if (collide_dir == DIR_LEFT) {
        angle = glm_vec3_angle(p->pos, UNIT_LEFT);
        x_dist = p->pos[0] - (-BOUND_X);
        y_dist = x_dist * tan(angle);
    } 

    vec3 collision_vec = { x_dist * unit_vel[0], y_dist * unit_vel[1], 0.0f};
    glm_vec3_add(p->pos, collision_vec, collision_pos);

    glm_vec3_copy(collision_pos, p->pos) ;

    return true;
}