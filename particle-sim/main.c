#include <stdlib.h>
#include <stdio.h>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#define MAX_SHADER_SOURCE_SIZE 5000

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
    LoadShader("frag.glsl", frag_shader_src, MAX_SHADER_SOURCE_SIZE);

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

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    int iResolutionLoc = glGetUniformLocation(program, "iResolution");

    while (!glfwWindowShouldClose(window)) {
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glUseProgram(program);
        glUniform3f(iResolutionLoc, 640.0, 480.0, 1.0);

        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, sizeof(indices)/sizeof(unsigned int), GL_UNSIGNED_INT, 0); 

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    return 0;
}