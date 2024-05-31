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

#include "include/cglm/struct/affine.h"
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

// Light
vec3s light_pos = {{0.0f, 1.0f, 2.0f}};

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

  Shader cube_shader = new_shader("./glsl/cube_vs.glsl", "./glsl/cube_fs.glsl");
  Shader lamp_shader = new_shader("./glsl/lamp_vs.glsl", "./glsl/lamp_fs.glsl");

  float vertices[] = {
      // positions          // normals           // texture coords
      -0.5f, -0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f, //
      0.5f, -0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f,  //
      0.5f, 0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 1.0f, 1.0f,   //
      0.5f, 0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 1.0f, 1.0f,   //
      -0.5f, 0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f,  //
      -0.5f, -0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f, //
                                                          //
      -0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,   //
      0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f,    //
      0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f,     //
      0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f,     //
      -0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f,    //
      -0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,   //
                                                          //
      -0.5f, 0.5f, 0.5f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f,   //
      -0.5f, 0.5f, -0.5f, -1.0f, 0.0f, 0.0f, 1.0f, 1.0f,  //
      -0.5f, -0.5f, -0.5f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f, //
      -0.5f, -0.5f, -0.5f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f, //
      -0.5f, -0.5f, 0.5f, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f,  //
      -0.5f, 0.5f, 0.5f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f,   //
                                                          //
      0.5f, 0.5f, 0.5f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f,     //
      0.5f, 0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f,    //
      0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f,   //
      0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f,   //
      0.5f, -0.5f, 0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f,    //
      0.5f, 0.5f, 0.5f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f,     //
                                                          //
      -0.5f, -0.5f, -0.5f, 0.0f, -1.0f, 0.0f, 0.0f, 1.0f, //
      0.5f, -0.5f, -0.5f, 0.0f, -1.0f, 0.0f, 1.0f, 1.0f,  //
      0.5f, -0.5f, 0.5f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f,   //
      0.5f, -0.5f, 0.5f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f,   //
      -0.5f, -0.5f, 0.5f, 0.0f, -1.0f, 0.0f, 0.0f, 0.0f,  //
      -0.5f, -0.5f, -0.5f, 0.0f, -1.0f, 0.0f, 0.0f, 1.0f, //
                                                          //
      -0.5f, 0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f,   //
      0.5f, 0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f,    //
      0.5f, 0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f,     //
      0.5f, 0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f,     //
      -0.5f, 0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f,    //
      -0.5f, 0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f    //
  };

  vec3s cube_positions[] = {
      {{0.0f, 0.0f, 0.0f}},     //
      {{2.0f, 5.0f, -15.0f}},   //
      {{-1.5f, -2.2f, -2.5f}},  //
      {{-3.8f, -2.0f, -12.3f}}, //
      {{2.4f, -0.4f, -3.5f}},   //
      {{-1.7f, 3.0f, -7.5f}},   //
      {{1.3f, -2.0f, -2.5f}},   //
      {{1.5f, 2.0f, -2.5f}},    //
      {{1.5f, 0.2f, -1.5f}},    //
      {{-1.3f, 1.0f, -1.5f}}    //
  };

  vec3s point_light_positions[] = {
      {{0.7f, 0.2f, 2.0f}},    //
      {{2.3f, -3.3f, -4.0f}},  //
      {{-4.0f, 2.0f, -12.0f}}, //
      {{0.0f, 0.0f, -3.0f}}    //
  };

  // VBO
  uint32_t VBO;

  glGenBuffers(1, &VBO);
  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

  // Cube
  uint32_t cube_VAO;

  glGenVertexArrays(1, &cube_VAO);
  glBindVertexArray(cube_VAO);

  glVertexAttribPointer(0, 3, GL_FLOAT, false, 8 * sizeof(float), NULL);
  glEnableVertexAttribArray(0);

  glVertexAttribPointer(1, 3, GL_FLOAT, false, 8 * sizeof(float),
                        (void *)(3 * sizeof(float)));
  glEnableVertexAttribArray(1);

  glVertexAttribPointer(2, 2, GL_FLOAT, false, 8 * sizeof(float),
                        (void *)(6 * sizeof(float)));
  glEnableVertexAttribArray(2);

  // Light
  uint32_t lamp_VAO;

  glGenVertexArrays(1, &lamp_VAO);
  glBindVertexArray(lamp_VAO);

  glBindBuffer(GL_ARRAY_BUFFER, VBO);

  glVertexAttribPointer(0, 3, GL_FLOAT, false, 8 * sizeof(float), NULL);
  glEnableVertexAttribArray(0);

  glBindVertexArray(0);

  // Camera
  camera = new_camera_default((vec3s){{0.0f, 0.0f, 3.0f}});

  // Texture
  uint32_t diffuseMap = generate_texture("./textures/container2.png");
  uint32_t specularMap = generate_texture("./textures/container2_specular.png");

  shader_use(&cube_shader);
  shader_set_int(&cube_shader, "material.diffuse", 0);
  shader_set_int(&cube_shader, "material.specular", 1);

  // Main rendering loop
  while (!glfwWindowShouldClose(window)) {
    float currentFrame = glfwGetTime();
    deltaTime = currentFrame - lastFrame;
    lastFrame = currentFrame;

    // Render here
    process_input(window);
    glClearColor(0x1e / 255.0, 0x29 / 255.0, 0x3b / 255.0, 1.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Active Shader
    shader_use(&cube_shader);
    shader_set_vec3(&cube_shader, "viewPos", camera.Position);

    // Cube material
    shader_set_float(&cube_shader, "material.shininess", 32.0f * 2);

    light_pos.x = sin(glfwGetTime()) * 2.0f;
    light_pos.z = cos(glfwGetTime()) * 1.0f;

    ////////////////////

    // Light

    // directional light
    shader_set_vec3(&cube_shader, "dir_light.direction",
                    (vec3s){{-0.2f, -1.0f, -0.3f}});
    shader_set_vec3(&cube_shader, "dir_light.ambient",
                    (vec3s){{0.05f, 0.05f, 0.05f}});
    shader_set_vec3(&cube_shader, "dir_light.diffuse",
                    (vec3s){{0.4f, 0.4f, 0.4f}});
    shader_set_vec3(&cube_shader, "dir_light.specular",
                    (vec3s){{0.5f, 0.5f, 0.5f}});
    // point light 1
    shader_set_vec3(&cube_shader, "point_lights[0].position",
                    point_light_positions[0]);
    shader_set_vec3(&cube_shader, "point_lights[0].ambient",
                    (vec3s){{0.05f, 0.05f, 0.05f}});
    shader_set_vec3(&cube_shader, "point_lights[0].diffuse",
                    (vec3s){{0.8f, 0.8f, 0.8f}});
    shader_set_vec3(&cube_shader, "point_lights[0].specular",
                    (vec3s){{1.0f, 1.0f, 1.0f}});
    shader_set_float(&cube_shader, "point_lights[0].constant", 1.0f);
    shader_set_float(&cube_shader, "point_lights[0].linear", 0.09f);
    shader_set_float(&cube_shader, "point_lights[0].quadratic", 0.032f);
    // point light 2
    shader_set_vec3(&cube_shader, "point_lights[1].position",
                    point_light_positions[1]);
    shader_set_vec3(&cube_shader, "point_lights[1].ambient",
                    (vec3s){{0.05f, 0.05f, 0.05f}});
    shader_set_vec3(&cube_shader, "point_lights[1].diffuse",
                    (vec3s){{0.8f, 0.8f, 0.8f}});
    shader_set_vec3(&cube_shader, "point_lights[1].specular",
                    (vec3s){{1.0f, 1.0f, 1.0f}});
    shader_set_float(&cube_shader, "point_lights[1].constant", 1.0f);
    shader_set_float(&cube_shader, "point_lights[1].linear", 0.09f);
    shader_set_float(&cube_shader, "point_lights[1].quadratic", 0.032f);
    // point light 3
    shader_set_vec3(&cube_shader, "point_lights[2].position",
                    point_light_positions[2]);
    shader_set_vec3(&cube_shader, "point_lights[2].ambient",
                    (vec3s){{0.05f, 0.05f, 0.05f}});
    shader_set_vec3(&cube_shader, "point_lights[2].diffuse",
                    (vec3s){{0.8f, 0.8f, 0.8f}});
    shader_set_vec3(&cube_shader, "point_lights[2].specular",
                    (vec3s){{1.0f, 1.0f, 1.0f}});
    shader_set_float(&cube_shader, "point_lights[2].constant", 1.0f);
    shader_set_float(&cube_shader, "point_lights[2].linear", 0.09f);
    shader_set_float(&cube_shader, "point_lights[2].quadratic", 0.032f);
    // point light 4
    shader_set_vec3(&cube_shader, "point_lights[3].position",
                    point_light_positions[3]);
    shader_set_vec3(&cube_shader, "point_lights[3].ambient",
                    (vec3s){{0.05f, 0.05f, 0.05f}});
    shader_set_vec3(&cube_shader, "point_lights[3].diffuse",
                    (vec3s){{0.8f, 0.8f, 0.8f}});
    shader_set_vec3(&cube_shader, "point_lights[3].specular",
                    (vec3s){{1.0f, 1.0f, 1.0f}});
    shader_set_float(&cube_shader, "point_lights[3].constant", 1.0f);
    shader_set_float(&cube_shader, "point_lights[3].linear", 0.09f);
    shader_set_float(&cube_shader, "point_lights[3].quadratic", 0.032f);
    // spotLight
    shader_set_vec3(&cube_shader, "spot_light.position", camera.Position);
    shader_set_vec3(&cube_shader, "spot_light.direction", camera.Front);
    shader_set_vec3(&cube_shader, "spot_light.ambient",
                    (vec3s){{0.0f, 0.0f, 0.0f}});
    shader_set_vec3(&cube_shader, "spot_light.diffuse",
                    (vec3s){{1.0f, 1.0f, 1.0f}});
    shader_set_vec3(&cube_shader, "spot_light.specular",
                    (vec3s){{1.0f, 1.0f, 1.0f}});
    shader_set_float(&cube_shader, "spot_light.constant", 1.0f);
    shader_set_float(&cube_shader, "spot_light.linear", 0.09f);
    shader_set_float(&cube_shader, "spot_light.quadratic", 0.032f);
    shader_set_float(&cube_shader, "spot_light.cutOff", cos(glm_rad(12.5f)));
    shader_set_float(&cube_shader, "spot_light.outerCutOff",
                     cos(glm_rad(15.0f)));

    ////////////////////

    // Cube Texture
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, diffuseMap);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, specularMap);

    // Transformations
    mat4s projection = glms_mat4_identity();
    projection =
        glms_perspective(glm_rad(camera.Fov),
                         (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
    shader_set_mat4(&cube_shader, "projection", projection);

    mat4s view = camera_get_view_matrix(&camera);
    shader_set_mat4(&cube_shader, "view", view);

    mat4s model = glms_mat4_identity();
    shader_set_mat4(&cube_shader, "model", model);

    glBindVertexArray(cube_VAO);
    // glDrawArrays(GL_TRIANGLES, 0, 36);

    for (unsigned int i = 0; i < 10; i++) {
      mat4s model = glms_mat4_identity();
      model = glms_translate(model, cube_positions[i]);
      float angle = 20.0f * i;
      model = glms_rotate(model, glm_rad(angle), (vec3s){{1.0f, 0.3f, 0.5f}});
      shader_set_mat4(&cube_shader, "model", model);

      glDrawArrays(GL_TRIANGLES, 0, 36);
    }

    // Lamp

    // shader_use(&lamp_shader);
    shader_set_mat4(&lamp_shader, "projection", projection);
    shader_set_mat4(&lamp_shader, "view", view);

    model = glms_mat4_identity();
    model = glms_translate(model, light_pos);
    model = glms_scale_uni(model, 0.2f);
    shader_set_mat4(&lamp_shader, "model", model);

    glBindVertexArray(lamp_VAO);
    glDrawArrays(GL_TRIANGLES, 0, 36);

    // Swap front and back buffers
    glfwSwapBuffers(window);
    // Poll for and process events
    glfwPollEvents();
  }

  glDeleteVertexArrays(1, &cube_VAO);
  glDeleteVertexArrays(1, &lamp_VAO);
  glDeleteBuffers(1, &VBO);
  shader_free(&cube_shader);
  shader_free(&lamp_shader);
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
