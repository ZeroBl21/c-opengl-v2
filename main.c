#include <GL/glew.h>
//
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>
#include <GLFW/glfw3.h>
#include <stdbool.h>
#include <stdio.h>

#define STB_IMAGE_IMPLEMENTATION
#include "include/stb_image.h"

#include "include/cglm/struct/affine-pre.h"
#include "include/cglm/struct/cam.h"
#include "include/cglm/struct/mat4.h"
#include "include/cglm/types-struct.h"

#define SHADER_IMPLEMENTATION
#include "lib/shader.h"
#define CAMERA_IMPLEMENTATION
#include "lib/camera.h"

void process_input(GLFWwindow *window);
void framebuffer_size_callback(GLFWwindow *window, int width, int height);
uint32_t generate_texture(const char *path);

void mouse_callback(GLFWwindow *window, double x_pos, double y_pos);
void scroll_callback(GLFWwindow *window, double x_pos, double y_pos);

// Settings
const size_t SCR_WIDTH = 800;
const size_t SCR_HEIGHT = 600;

// Camera
Camera camera;
float last_x = (float)SCR_WIDTH / 2;
float last_y = (float)SCR_HEIGHT / 2;
bool firstMouse = true;

// Timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;

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

  // Capture Cursor
  glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
  glfwSetCursorPosCallback(window, mouse_callback);
  glfwSetScrollCallback(window, scroll_callback);

  glewExperimental = true;
  glewInit();

  glEnable(GL_DEPTH_TEST);

  Shader base_shader = new_shader("./glsl/vs.glsl", "./glsl/fs.glsl");

  float vertices[] = {
      -0.5f, -0.5f, -0.5f, 0.0f, 0.0f, 0.5f,  -0.5f, -0.5f, 1.0f, 0.0f,
      0.5f,  0.5f,  -0.5f, 1.0f, 1.0f, 0.5f,  0.5f,  -0.5f, 1.0f, 1.0f,
      -0.5f, 0.5f,  -0.5f, 0.0f, 1.0f, -0.5f, -0.5f, -0.5f, 0.0f, 0.0f,

      -0.5f, -0.5f, 0.5f,  0.0f, 0.0f, 0.5f,  -0.5f, 0.5f,  1.0f, 0.0f,
      0.5f,  0.5f,  0.5f,  1.0f, 1.0f, 0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
      -0.5f, 0.5f,  0.5f,  0.0f, 1.0f, -0.5f, -0.5f, 0.5f,  0.0f, 0.0f,

      -0.5f, 0.5f,  0.5f,  1.0f, 0.0f, -0.5f, 0.5f,  -0.5f, 1.0f, 1.0f,
      -0.5f, -0.5f, -0.5f, 0.0f, 1.0f, -0.5f, -0.5f, -0.5f, 0.0f, 1.0f,
      -0.5f, -0.5f, 0.5f,  0.0f, 0.0f, -0.5f, 0.5f,  0.5f,  1.0f, 0.0f,

      0.5f,  0.5f,  0.5f,  1.0f, 0.0f, 0.5f,  0.5f,  -0.5f, 1.0f, 1.0f,
      0.5f,  -0.5f, -0.5f, 0.0f, 1.0f, 0.5f,  -0.5f, -0.5f, 0.0f, 1.0f,
      0.5f,  -0.5f, 0.5f,  0.0f, 0.0f, 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

      -0.5f, -0.5f, -0.5f, 0.0f, 1.0f, 0.5f,  -0.5f, -0.5f, 1.0f, 1.0f,
      0.5f,  -0.5f, 0.5f,  1.0f, 0.0f, 0.5f,  -0.5f, 0.5f,  1.0f, 0.0f,
      -0.5f, -0.5f, 0.5f,  0.0f, 0.0f, -0.5f, -0.5f, -0.5f, 0.0f, 1.0f,

      -0.5f, 0.5f,  -0.5f, 0.0f, 1.0f, 0.5f,  0.5f,  -0.5f, 1.0f, 1.0f,
      0.5f,  0.5f,  0.5f,  1.0f, 0.0f, 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
      -0.5f, 0.5f,  0.5f,  0.0f, 0.0f, -0.5f, 0.5f,  -0.5f, 0.0f, 1.0f};

  vec3s cubePositions[] = {{{0.0f, 0.0f, 0.0f}},    {{2.0f, 5.0f, -15.0f}},
                           {{-1.5f, -2.2f, -2.5f}}, {{-3.8f, -2.0f, -12.3f}},
                           {{2.4f, -0.4f, -3.5f}},  {{-1.7f, 3.0f, -7.5f}},
                           {{1.3f, -2.0f, -2.5f}},  {{1.5f, 2.0f, -2.5f}},
                           {{1.5f, 0.2f, -1.5f}},   {{-1.3f, 1.0f, -1.5f}}};

  uint32_t VBO, VAO;

  // VAO
  glGenVertexArrays(1, &VAO);
  glBindVertexArray(VAO);

  // VBO
  glGenBuffers(1, &VBO);
  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

  glVertexAttribPointer(0, 3, GL_FLOAT, false, 5 * sizeof(float), NULL);
  glEnableVertexAttribArray(0);

  glVertexAttribPointer(1, 2, GL_FLOAT, false, 5 * sizeof(float),
                        (void *)(3 * sizeof(float)));
  glEnableVertexAttribArray(1);

  glBindVertexArray(0);

  // Camera
  camera = new_camera_default((vec3s){{0.0f, 0.0f, 3.0f}});

  // Textures
  uint32_t tContainer = generate_texture("./textures/container.jpg");
  uint32_t tFace = generate_texture("./textures/awesome_face.png");

  shader_use(&base_shader);
  shader_set_int(&base_shader, "texture1", 0);
  shader_set_int(&base_shader, "texture2", 1);

  // Main rendering loop
  while (!glfwWindowShouldClose(window)) {
    float currentFrame = glfwGetTime();
    deltaTime = currentFrame - lastFrame;
    lastFrame = currentFrame;

    // Render here
    process_input(window);
    glClearColor(0x1e / 255.0, 0x29 / 255.0, 0x3b / 255.0, 1.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, tContainer);

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, tFace);

    // Active Shader
    shader_use(&base_shader);

    // Transformations
    mat4s projection = glms_mat4_identity();
    projection =
        glms_perspective(glm_rad(camera.Fov),
                         (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
    shader_set_mat4(&base_shader, "projection", projection);

    mat4s view = camera_get_view_matrix(&camera);
    view = glms_translate(view, (vec3s){{0.0f, 0.0f, -3.0f}});
    shader_set_mat4(&base_shader, "view", view);

    // Render Container
    glBindVertexArray(VAO);
    for (size_t i = 0; i < 10; i++) {
      mat4s model = glms_mat4_identity();
      model = glms_translate(model, cubePositions[i]);

      float angle = 20.0f * glfwGetTime() + i * 10;
      model = glms_rotate(model, glm_rad(angle), (vec3s){{1.0f, 0.3f, 0.5f}});
      shader_set_mat4(&base_shader, "model", model);

      glDrawArrays(GL_TRIANGLES, 0, 36);
    }

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

  // Movement
  if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
    camera_process_keyboard(&camera, FORWARD, deltaTime);
  }
  if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
    camera_process_keyboard(&camera, BACKWARD, deltaTime);
  }
  if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {

    camera_process_keyboard(&camera, LEFT, deltaTime);
  }
  if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
    camera_process_keyboard(&camera, RIGHT, deltaTime);
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

void mouse_callback(GLFWwindow *window, double x_pos, double y_pos) {
  (void)window;

  if (firstMouse) {
    last_x = x_pos;
    last_y = y_pos;
    firstMouse = false;
  }

  float x_offset = x_pos - last_x;
  // Reversed since y-coordinates range from bottom to top
  float y_offset = last_y - y_pos;

  last_x = x_pos;
  last_y = y_pos;

  camera_process_mouse_movement(&camera, x_offset, y_offset, true);
}

void scroll_callback(GLFWwindow *window, double x_offset, double y_offset) {
  (void)window;
  (void)x_offset;

  camera_process_mouse_scroll(&camera, (float)y_offset);
}
