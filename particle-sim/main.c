#include <stdlib.h>
#include <stdio.h>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <cglm/cglm.h> 
#include "particle.h"
#include <windows.h>

#define MAX_SHADER_SOURCE_SIZE 5000
#define PARTICLE_COUNT 100

void CheckGLErrors(char* label) {
    GLenum err;
    while((err = glGetError()) != GL_NO_ERROR)
    {
        fprintf(stderr, "[%s] Unhandled GL error: %u\n", label, err);
    }

}

char* LoadShader(const char* filepath, char* contents, size_t max_content_size) {
    FILE *shader_file;
    shader_file = fopen(filepath, "r");
    if (!shader_file)
    {
        fprintf(stderr, "Error opening file %s", filepath);
        exit(EXIT_FAILURE);
    }

    size_t read_count = fread(contents, sizeof(char), max_content_size - 1, shader_file);
    contents[read_count] = '\0';
}

void StepVerlet(float delta, Particle *p) {
    vec3 acc = {0.0, 0.0, 0.0};
    vec3 prev_pos;
    glm_vec3_copy(p->pos_prev, prev_pos);
    glm_vec3_copy(p->pos, p->pos_prev);

    // p->pos[0] = p->pos[0] * 2 - prev0 + acc * delta * delta;
    // p->pos[1] = p->pos[1] * 2 - prev1 + acc * delta * delta;
    glm_vec3_scale(p->pos, 2, p->pos);
    glm_vec3_sub(p->pos, prev_pos, p->pos);
    glm_vec3_scale(acc, delta*delta, acc);
    glm_vec3_add(p->pos, acc, p->pos);
}

float rand_f() {
    return (float)rand()/(float)(RAND_MAX/1.0);
}

int main() {
    if (glfwInit() != GLFW_TRUE)
    {
        glfwTerminate();
        return -1;
    }

    GLFWwindow* window = glfwCreateWindow(640, 480, "Particle Sim", NULL, NULL);
 	glfwMakeContextCurrent(window);
	if (!gladLoadGLLoader((GLADloadproc) glfwGetProcAddress)) {
		fprintf(stderr, "Failed to initialize glad\n");
	}   

    // --- Load shaders ---
    char vert_shader_src[MAX_SHADER_SOURCE_SIZE];
    char frag_shader_src[MAX_SHADER_SOURCE_SIZE];
    LoadShader("vert.glsl", vert_shader_src, MAX_SHADER_SOURCE_SIZE);
    LoadShader("frag2.glsl", frag_shader_src, MAX_SHADER_SOURCE_SIZE);

    GLuint vert_shader = glCreateShader(GL_VERTEX_SHADER);
    if (!vert_shader) {
        fprintf(stderr, "Failed to create vertex shader\n");
    }
    CheckGLErrors("Compiled vert shader 01");
    char* vert_p = &vert_shader_src[0];
    glShaderSource(vert_shader, 1, &vert_p, NULL);
    CheckGLErrors("Compiled vert shader 02");
    glCompileShader(vert_shader);
    CheckGLErrors("Compiled vert shader 03");

    GLuint frag_shader = glCreateShader(GL_FRAGMENT_SHADER);
    if (!frag_shader) {
        fprintf(stderr, "Failed to create vertex shader\n");
    }
    char* frag_p = &frag_shader_src[0];
    glShaderSource(frag_shader, 1, &frag_p, NULL);
    glCompileShader(frag_shader);
    CheckGLErrors("Compiled frag shader");

    GLuint program = glCreateProgram();
    glAttachShader(program, vert_shader);
    glAttachShader(program, frag_shader);
    glLinkProgram(program);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    int iResolutionLoc = glGetUniformLocation(program, "iResolution");

    // -------- particles ----------
    int modelLoc = glGetUniformLocation(program, "model");
    int viewLoc = glGetUniformLocation(program, "view");
    int projectionLoc = glGetUniformLocation(program, "projection");

    mat4 view = GLM_MAT4_IDENTITY_INIT;
    mat4 projection = GLM_MAT4_IDENTITY_INIT;

    // view
    // TODO: extract out to Camera struct
    vec3 camera_pos = { 0.0f, 0.0f, 10.0f};
    vec3 camera_up = { 0.0f, 1.0f, 0.0f};
    vec3 camera_front = { 0.0f, 0.0f, -1.0f};
    vec3 lookat_center;
    glm_vec3_add(camera_pos, camera_front, lookat_center);
    glm_lookat(camera_pos, lookat_center, camera_up, view);

    // projection
    glm_perspective(glm_rad(100.0f), 1280.0f / 720.0f, 0.1f, 100.0f, projection);

    glUseProgram(program);
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, (float *) view);
    glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, (float *) projection);
    glUniform3f(iResolutionLoc, 640.0, 480.0, 1.0);

    vec3 scale = {0.5, 0.5, 1.0};
    // model pos
    Particle *ps[PARTICLE_COUNT];
    for (int i=0; i<PARTICLE_COUNT; i++) {
        float x = rand_f();
        float y = rand_f();
        float velx = rand_f() / 10.0;
        float vely = rand_f() / 10.0;
        // Particle p = {.pos = {x, y, 0.0}, .pos_prev = {x-velx, y-vely, 0.0}, .vel = {velx, vely, 0.0} };
        Particle *p = ParticleNew();
        p->pos[0] = x;
        p->pos[1] = y;
        p->pos_prev[0] = x-velx;
        p->pos_prev[1] = y-vely;
        p->vel[0] = velx;
        p->vel[1] = vely;

        ps[i] = p;
    }

    LARGE_INTEGER frequency;
    LARGE_INTEGER t1;

    double delta;
    QueryPerformanceFrequency(&frequency);
    QueryPerformanceCounter(&t1);
    double start = t1.QuadPart;

    while (!glfwWindowShouldClose(window)) {
        QueryPerformanceCounter(&t1);
        delta = (t1.QuadPart - start) / frequency.QuadPart;
        start = t1.QuadPart;

        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        Particle *p;
        for (int i=0; i<PARTICLE_COUNT; i++) {
            p = ps[i];
            StepVerlet(delta, p);
            glBindVertexArray(p->VAO);
            mat4 model = GLM_MAT4_IDENTITY_INIT;
            glm_translate(model, p->pos);
            glm_scale(model, scale);
            glUniformMatrix4fv(modelLoc, 1, GL_FALSE, (float *) model);
            glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0); 
        }

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    for (int i=0; i<PARTICLE_COUNT; i++) {
        free(ps[i]);
    }

    return 0;
}