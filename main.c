#include "include/cglm/struct/mat4.h"
#include "include/cglm/types-struct.h"
#include <GL/glew.h>
//
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>
#include <GLFW/glfw3.h>
#include <stdbool.h>
#include <stdio.h>

#define STB_IMAGE_IMPLEMENTATION
#include "include/cglm/struct/affine-pre.h"
#include "include/stb_image.h"

#define SHADER_IMPLEMENTATION
#include "lib/shader.h"

void process_input(GLFWwindow *window);
void framebuffer_size_callback(GLFWwindow *window, int width, int height);
void check_shader_compilation(GLuint shader, const char *shaderType,
                              const char *filename);
void check_program_linking(GLuint programID);
uint32_t generate_texture(const char *path);

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

  float vertices[] = {
      // positions          // colors           // texture coords
      0.5f,  0.5f,  0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, // top right
      0.5f,  -0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, // bottom right
      -0.5f, -0.5f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, // bottom left
      -0.5f, 0.5f,  0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f  // top left
  };

  uint32_t indices[] = {
      0, 1, 3, // first triangle
      1, 2, 3  // second triangle
  };

  uint32_t VBO, VAO, EBO;

  // VAO
  glGenVertexArrays(1, &VAO);
  glBindVertexArray(VAO);

  // VBO
  glGenBuffers(1, &VBO);
  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

  glGenBuffers(1, &EBO);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices,
               GL_STATIC_DRAW);

  glVertexAttribPointer(0, 3, GL_FLOAT, false, 8 * sizeof(float), NULL);
  glEnableVertexAttribArray(0);

  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float),
                        (void *)(3 * sizeof(float)));
  glEnableVertexAttribArray(1);

  glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float),
                        (void *)(6 * sizeof(float)));
  glEnableVertexAttribArray(2);

  glBindVertexArray(0);

  // Textures
  uint32_t tContainer = generate_texture("./textures/container.jpg");
  uint32_t tFace = generate_texture("./textures/awesome_face.png");

  shader_use(&base_shader);
  shader_set_int(&base_shader, "texture1", 0);
  shader_set_int(&base_shader, "texture2", 1);

  // Main rendering loop
  while (!glfwWindowShouldClose(window)) {
    // Render here
    process_input(window);
    glClearColor(0x1e / 255.0, 0x29 / 255.0, 0x3b / 255.0, 1.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, tContainer);

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, tFace);

    // Transformations
    mat4s transform = glms_mat4_identity();
    transform = glms_translate(transform, (vec3s){{0.5f, -0.5f, 0.0f}});
    transform = glms_rotate(transform, (float)glfwGetTime(),
                            (vec3s){{0.0f, 0.0f, 1.0f}});
    // Transformations

    shader_use(&base_shader);
    shader_set_mat4(&base_shader, "transform", transform);

    // Render Container
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

    // Swap front and back buffers
    glfwSwapBuffers(window);
    // Poll for and process events
    glfwPollEvents();
  }

  glDeleteVertexArrays(1, &VAO);
  glDeleteBuffers(1, &VBO);
  glDeleteBuffers(1, &EBO);
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

uint32_t generate_texture(const char *path) {
  stbi_set_flip_vertically_on_load(true);

  uint32_t texture;
  glGenTextures(1, &texture);
  glBindTexture(GL_TEXTURE_2D, texture);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
                  GL_LINEAR_MIPMAP_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  int32_t width, height, nr_channels;
  uint8_t *data = stbi_load(path, &width, &height, &nr_channels, 0);
  if (!data) {
    fprintf(stderr, "ERROR: Failed to load texture at path: %s\n", path);
    stbi_image_free(data);
    return 0;
  }

  GLenum format;
  if (nr_channels == 1)
    format = GL_RED;
  if (nr_channels == 3)
    format = GL_RGB;
  if (nr_channels == 4)
    format = GL_RGBA;

  glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format,
               GL_UNSIGNED_BYTE, data);
  glGenerateMipmap(GL_TEXTURE_2D);

  stbi_image_free(data);
  return texture;
}
