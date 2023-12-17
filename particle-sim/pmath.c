#include <assert.h>
#include "pmath.h"

float mirror_across_y(vec3 v, float mirror_point) {
    float delta = v[1] - mirror_point;
    v[1] = mirror_point - delta;
}

float mirror_across_x(vec3 v, float mirror_point) {
    float delta = v[0] - mirror_point;
    v[0] = mirror_point - delta;
}

void HandleCollision(Particle *p, vec3 *next_pos, Dir collide_dir) {
    if (collide_dir == DIR_NONE) {
        return false;
    }

    // ---------------------------------------
    // TODO -- remove this impromptu testing
    // Test mirror_across_y
    vec3 test_vec = {1.0f, 10.0f, 0.0f};
    mirror_across_y(test_vec, 6.0f);
    assert(test_vec[1] == 2.0f);
    vec3 test_vec2 = {1.0f, 3.5f, 0.0f};
    mirror_across_y(test_vec2, 6.0f);
    assert(test_vec2[1] == 8.5f);
    // ---------------------------------------

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
    } else if (collide_dir == DIR_LEFT) {
        angle = glm_vec3_angle(p->pos, UNIT_LEFT);
        x_dist = p->pos[0] - (-BOUND_X);
        y_dist = x_dist * tan(angle);
    } else if (collide_dir == DIR_RIGHT) {
        angle = glm_vec3_angle(p->pos, UNIT_RIGHT);
        x_dist = BOUND_X - p->pos[0];
        y_dist = x_dist * tan(angle);
    }

    // vec from pos -> collision point
    vec3 collision_into_vec = { x_dist * unit_vel[0], y_dist * unit_vel[1], 0.0f};
    glm_vec3_add(p->pos, collision_into_vec, collision_pos);

    vec3 reflected_next_pos;
    glm_vec3_copy(next_pos, reflected_next_pos);

    vec3 reflected_prev_pos;
    glm_vec3_copy(p->pos, reflected_prev_pos);

    if (collide_dir == DIR_TOP || collide_dir == DIR_BOT) {
        mirror_across_y(reflected_next_pos, collision_pos[1]);
        mirror_across_y(reflected_prev_pos, collision_pos[1]);

    } else if (collide_dir == DIR_LEFT || DIR_RIGHT) {
        mirror_across_x(reflected_next_pos, collision_pos[0]);
        mirror_across_x(reflected_prev_pos, collision_pos[0]);
    }

    glm_vec3_copy(reflected_next_pos, p->pos);
    glm_vec3_copy(reflected_prev_pos, p->pos_prev);

    return true;
}