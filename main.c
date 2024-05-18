#include <GL/glew.h>
//
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>
#include <GLFW/glfw3.h>
#include <stdbool.h>
#include <stdio.h>

#include "lib/file.h"

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

  arena shaderArena = new_arena(1024);

  string vShaderF = read_file("./glsl/vs.glsl", &shaderArena);
  uint32_t vShader = glCreateShader(GL_VERTEX_SHADER);
  glShaderSource(vShader, 1, (const char *const *)&vShaderF.data, NULL);
  glCompileShader(vShader);

  check_shader_compilation(vShader, "Vertex", "./glsl/vs.glsl");

  string fShaderF = read_file("./glsl/fs.glsl", &shaderArena);
  uint32_t fShader = glCreateShader(GL_FRAGMENT_SHADER);
  glShaderSource(fShader, 1, (const char *const *)&fShaderF.data, NULL);
  glCompileShader(fShader);

  check_shader_compilation(vShader, "Fragment", "./glsl/fs.glsl");

  arena_free(&shaderArena);

  uint32_t sProgram = glCreateProgram();
  glAttachShader(sProgram, vShader);
  glAttachShader(sProgram, fShader);
  glLinkProgram(sProgram);

  check_program_linking(sProgram);

  glDeleteShader(vShader);
  glDeleteShader(fShader);

  float vertices[] = {
      0.5f,  0.5f,  0.0f, // top right
      0.5f,  -0.5f, 0.0f, // bottom right
      -0.5f, -0.5f, 0.0f, // bottom left
      -0.5f, 0.5f,  0.0f  // top left
  };
  uint32_t indices[] = {
      // note that we start from 0!
      0, 1, 3, // first Triangle
      1, 2, 3  // second Triangle
  };

  uint32_t VBO, VAO, EBO;

  // VAO
  glGenVertexArrays(1, &VAO);
  glBindVertexArray(VAO);

  // VBO
  glGenBuffers(1, &VBO);
  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

  // EBO
  glGenBuffers(1, &EBO);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices,
               GL_STATIC_DRAW);

  glVertexAttribPointer(0, 3, GL_FLOAT, false, 3 * sizeof(float), NULL);
  glEnableVertexAttribArray(0);

  glBindBuffer(GL_ARRAY_BUFFER, 0);

  glBindVertexArray(0);

  // Main rendering loop
  while (!glfwWindowShouldClose(window)) {
    // Render here
    process_input(window);
    glClearColor(0x1e / 255.0, 0x29 / 255.0, 0x3b / 255.0, 1.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glUseProgram(sProgram);
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, sizeof(indices), GL_UNSIGNED_INT, 0);

    // Swap front and back buffers
    glfwSwapBuffers(window);
    // Poll for and process events
    glfwPollEvents();
  }

  glDeleteVertexArrays(1, &VAO);
  glDeleteBuffers(1, &VBO);
  glDeleteProgram(sProgram);
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

void check_shader_compilation(GLuint shader, const char *shaderType,
                              const char *filename) {
  int success;
  char infoLog[512];
  glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
  if (!success) {
    glGetShaderInfoLog(shader, 512, NULL, infoLog);
    fprintf(stderr,
            "ERROR: %s shader compilation failed\nFile: %s\nDetails:\n%s\n",
            shaderType, filename, infoLog);
  }
}

void check_program_linking(GLuint programID) {
  int success;
  char infoLog[512];
  glGetProgramiv(programID, GL_LINK_STATUS, &success);
  if (!success) {
    glGetProgramInfoLog(programID, 512, NULL, infoLog);
    fprintf(stderr, "ERROR: Shader program linking failed %s\n", infoLog);
  }
}
