#include <GL/glew.h>
//
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>
#include <GLFW/glfw3.h>
#include <stdbool.h>
#include <stdio.h>

#define SHADER_IMPLEMENTATION
#include "lib/shader.h"

void process_input(GLFWwindow *window);
void framebuffer_size_callback(GLFWwindow *window, int width, int height);
void check_shader_compilation(GLuint shader, const char *shaderType,
                              const char *filename);
void check_program_linking(GLuint programID);

const size_t SCR_WIDTH = 800;
const size_t SCR_HEIGHT = 600;

int main(void) {
  if (!glfwInit()) {
    fprintf(stderr, "ERROR: Failed to initialize GLFW\n");
    return -1;
  }

  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  // Create a GLFW window
  GLFWwindow *window =
      glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Mesa with GLFW", NULL, NULL);
  if (!window) {
    fprintf(stderr, "ERROR: Failed to create GLFW window\n");
    glfwTerminate();
    return -1;
  }

  // Make the window's context current
  glfwMakeContextCurrent(window);
  glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

  glewExperimental = true;
  glewInit();

  Shader base_shader = new_shader("./glsl/vs.glsl", "./glsl/fs.glsl");

  // float vertices[] = {
  //     // positions         // colors
  //     0.5f,  -0.5f, 0.0f, 1.0f, 0.0f, 0.0f, // bottom right
  //     -0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f, // bottom left
  //     0.0f,  0.5f,  0.0f, 0.0f, 0.0f, 1.0f  // top
  // };

  // Upside Down
  float vertices[] = {
      // positions         // colors
      0.5f,  0.5f,  0.0f, 1.0f, 0.0f, 0.0f, // bottom right
      -0.5f, 0.5f,  0.0f, 0.0f, 1.0f, 0.0f, // bottom left
      0.0f,  -0.5f, 0.0f, 0.0f, 0.0f, 1.0f  // top
  };

  uint32_t VBO, VAO;

  // VAO
  glGenVertexArrays(1, &VAO);
  glBindVertexArray(VAO);

  // VBO
  glGenBuffers(1, &VBO);
  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

  glVertexAttribPointer(0, 3, GL_FLOAT, false, 6 * sizeof(float), NULL);
  glEnableVertexAttribArray(0);

  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float),
                        (void *)(3 * sizeof(float)));
  glEnableVertexAttribArray(1);

  glBindVertexArray(0);

  // Main rendering loop
  while (!glfwWindowShouldClose(window)) {
    // Render here
    process_input(window);
    glClearColor(0x1e / 255.0, 0x29 / 255.0, 0x3b / 255.0, 1.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    shader_use(&base_shader);

    glBindVertexArray(VAO);
    glDrawArrays(GL_TRIANGLES, 0, 3);

    // Swap front and back buffers
    glfwSwapBuffers(window);
    // Poll for and process events
    glfwPollEvents();
  }

  glDeleteVertexArrays(1, &VAO);
  glDeleteBuffers(1, &VBO);
  shader_free(&base_shader);
  // Cleanup GLFW
  glfwTerminate();

  return EXIT_SUCCESS;
}

void process_input(GLFWwindow *window) {
  if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS) {
    glfwSetWindowShouldClose(window, true);
  }

  if (glfwGetKey(window, GLFW_KEY_COMMA) == GLFW_PRESS) {
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
  }

  if (glfwGetKey(window, GLFW_KEY_PERIOD) == GLFW_PRESS) {
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
  }
}

void framebuffer_size_callback(GLFWwindow *window, int width, int height) {
  (void)window;
  (void)width;
  (void)height;
  glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
}
