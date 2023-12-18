#include <glad/glad.h>

#include "line.h"

Line* NewLine(vec3 start, vec3 end) {
    Line* line = malloc(sizeof(Line));
    glm_vec3_copy(start, line->start);
    glm_vec3_copy(end, line->end);
    
    float vertices[6] = {
        line->start[0], line->start[1], line->start[2],
        line->end[0], line->end[1], line->end[2]
    };

    GLuint VAO;
    GLuint VBO;

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), &vertices[0], GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*) 0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    line->VAO = VAO;

    return line;
}