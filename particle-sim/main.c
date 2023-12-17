#include <stdlib.h>
#include <stdio.h>
#include <windows.h>

#include <cglm/cglm.h> 
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "const.h"
#include "particle.h"
#include "pmath.h"
#include "types.h"

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

    vec3 next_pos;

    glm_vec3_scale(p->pos, 2, next_pos);
    glm_vec3_sub(next_pos, prev_pos, next_pos);
    glm_vec3_scale(acc, delta*delta, acc);
    glm_vec3_add(next_pos, acc, next_pos);

    Dir collide_dir = DIR_NONE;
    // calculate bouncing off edges
    if (next_pos[0] > BOUND_X) {
        collide_dir = DIR_RIGHT;
    } else if (next_pos[0] < -BOUND_X) {
        collide_dir = DIR_LEFT;
    } else if (next_pos[1] > BOUND_Y) {
        collide_dir = DIR_TOP;
    } else if (next_pos[1] < -BOUND_Y) {
        collide_dir = DIR_BOT;
    }

    if (collide_dir != DIR_NONE) {
        HandleCollision(p, &next_pos, collide_dir);
    } else {
        glm_vec3_copy(next_pos, p->pos);
    }
}

float rand_f() {
    return (float)rand()/(float)(RAND_MAX/1.0) - 0.5;
}

Particle* CreateBoundaryParticles(Particle **boundary_particles) {
    Particle *top = ParticleNew();
    top->pos[1] = BOUND_Y;
    boundary_particles[0] = top;

    Particle *bot = ParticleNew();
    bot->pos[1] = -BOUND_Y;
    boundary_particles[1] = bot;

    Particle *right = ParticleNew();
    right->pos[0] = BOUND_X;
    boundary_particles[2] = right;

    Particle *left = ParticleNew();
    left->pos[0] = -BOUND_X;
    boundary_particles[3] = left;

    return boundary_particles;
}

int main() {
    srand(RAND_SEED);

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
    // for (int i=0; i<PARTICLE_COUNT; i++) {
    //     float x = rand_f();
    //     float y = rand_f();
    //     float velx = rand_f() / 10.0;
    //     float vely = rand_f() / 10.0;
    //     // Particle p = {.pos = {x, y, 0.0}, .pos_prev = {x-velx, y-vely, 0.0}, .vel = {velx, vely, 0.0} };
    //     Particle *p = ParticleNew();
    //     p->pos[0] = x;
    //     p->pos[1] = y;
    //     p->pos_prev[0] = x-velx;
    //     p->pos_prev[1] = y-vely;
    //     p->vel[0] = velx;
    //     p->vel[1] = vely;

    //     ps[i] = p;
    // }

    // --- TESTING ONLY ---
    // Particle p = {.pos = {x, y, 0.0}, .pos_prev = {x-velx, y-vely, 0.0}, .vel = {velx, vely, 0.0} };
    Particle *p = ParticleNew();
    float velx = 0.0;
    float vely = -0.1;
    p->pos_prev[0] = p->pos[0]-velx;
    p->pos_prev[1] = p->pos[1]-vely;
    p->vel[0] = velx;
    p->vel[1] = vely;
    ps[0] = p;

    Particle* boundary_particles[4];
    CreateBoundaryParticles(boundary_particles);
    for (int i=0; i<4; i++) {
        ps[i+1] = boundary_particles[i];
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