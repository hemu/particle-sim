#include <cglm/cglm.h>
#include <glad/glad.h>

#include "particle.h"

Particle* ParticleNew() {
    float vertices[12] = {
        -0.5, 0.5, 0.0,
        0.5, 0.5, 0.0,
        0.5, -0.5, 0.0,
        -0.5, -0.5, 0.0
    };

    unsigned int indices[6] = {
        0, 1, 2,
        0, 2, 3
    };

    const int vertex_count = sizeof(vertices) / sizeof(float);

    GLuint VAO;
    GLuint VBO;
    GLuint EBO;

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), &vertices[0], GL_STATIC_DRAW);
    
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), &indices[0], GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*) 0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    Particle *p = malloc(sizeof(Particle));
    if (p == NULL) {
        return NULL;
    }

    p->pos[0] = 0.0f;
    p->pos[1] = 0.0f;
    p->pos[2] = 0.0f;

    p->pos_prev[0] = 0.0f;
    p->pos_prev[1] = 0.0f;
    p->pos_prev[2] = 0.0f;

    p->vel[0] = 0.0f;
    p->vel[1] = 0.0f;
    p->vel[2] = 0.0f;

    p->VAO = VAO;

    return p;
}