#ifndef SHADER_H
#define SHADER_H

#include <GL/glew.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "../include/cglm/types-struct.h"
#include "file.h"

typedef struct {
  GLuint ID;
} Shader;

Shader new_shader(const char *vertex_path, const char *fragment_path);
void shader_free(Shader *shader);
void shader_use(Shader *shader);
// Primitives
void shader_set_bool(Shader *shader, const char *name, bool value);
void shader_set_int(Shader *shader, const char *name, int value);
void shader_set_float(Shader *shader, const char *name, float value);
// Vectors
void shader_set_vec2(Shader *shader, const char *name, const vec2s value);
void shader_set_vec2f(Shader *shader, const char *name, float x, float y);
void shader_set_vec3(Shader *shader, const char *name, const vec3s value);
void shader_set_vec3f(Shader *shader, const char *name, float x, float y,
                      float z);
void shader_set_vec4(Shader *shader, const char *name, const vec4s value);
void shader_set_vec4f(Shader *shader, const char *name, float x, float y,
                      float z, float w);
// Matrices
void shader_set_mat2(Shader *shader, const char *name, const mat2s mat);
void shader_set_mat3(Shader *shader, const char *name, const mat3s mat);
void shader_set_mat4(Shader *shader, const char *name, const mat4s mat);

// Helper
void check_shader_compilation(GLuint shader, const char *shader_type,
                              const char *filename);
void check_program_linking(GLuint programID);
#endif // SHADER_H

// #define SHADER_IMPLEMENTATION
#ifdef SHADER_IMPLEMENTATION
inline Shader new_shader(const char *vertex_path, const char *fragment_path) {
  Shader shader = {0};

  // 1. Retrieve the vertex/fragment source code from filePath
  arena sArena = new_arena(1024);

  string vertex_code = read_file(vertex_path, &sArena);
  string fragment_code = read_file(fragment_path, &sArena);

  // return shader;
  GLuint vertex, fragment;

  vertex = glCreateShader(GL_VERTEX_SHADER);
  glShaderSource(vertex, 1, (const char *const *)&vertex_code.data, NULL);
  glCompileShader(vertex);

  check_shader_compilation(vertex, "Vertex", vertex_path);

  fragment = glCreateShader(GL_FRAGMENT_SHADER);
  glShaderSource(fragment, 1, (const char *const *)&fragment_code.data, NULL);
  glCompileShader(fragment);

  check_shader_compilation(fragment, "Fragment", fragment_path);

  shader.ID = glCreateProgram();
  glAttachShader(shader.ID, vertex);
  glAttachShader(shader.ID, fragment);
  glLinkProgram(shader.ID);

  check_program_linking(shader.ID);

  glDeleteShader(vertex);
  glDeleteShader(fragment);
  arena_free(&sArena);

  return shader;
}

inline void shader_use(Shader *shader) { glUseProgram(shader->ID); }

inline void shader_free(Shader *shader) { glDeleteProgram(shader->ID); }

inline void shader_set_bool(Shader *shader, const char *name, bool value) {
  glUniform1i(glGetUniformLocation(shader->ID, name), (int)value);
}

inline void shader_set_int(Shader *shader, const char *name, int value) {
  glUniform1i(glGetUniformLocation(shader->ID, name), value);
}

inline void shader_set_float(Shader *shader, const char *name, float value) {
  glUniform1f(glGetUniformLocation(shader->ID, name), value);
}
// ------------------------------------------------------------------------
inline void shader_set_vec2(Shader *shader, const char *name,
                            const vec2s value) {
  glUniform2fv(glGetUniformLocation(shader->ID, name), 1, value.raw);
}

inline void shader_set_vec2f(Shader *shader, const char *name, float x,
                             float y) {
  glUniform2f(glGetUniformLocation(shader->ID, name), x, y);
}
// ------------------------------------------------------------------------
inline void shader_set_vec3(Shader *shader, const char *name,
                            const vec3s value) {
  glUniform3fv(glGetUniformLocation(shader->ID, name), 1, value.raw);
}
inline void shader_set_vec3f(Shader *shader, const char *name, float x, float y,
                             float z) {
  glUniform3f(glGetUniformLocation(shader->ID, name), x, y, z);
}
// ------------------------------------------------------------------------
inline void shader_set_vec4(Shader *shader, const char *name,
                            const vec4s value) {
  glUniform4fv(glGetUniformLocation(shader->ID, name), 1, value.raw);
}
inline void shader_set_vec4f(Shader *shader, const char *name, float x, float y,
                             float z, float w) {
  glUniform4f(glGetUniformLocation(shader->ID, name), x, y, z, w);
}
// ------------------------------------------------------------------------
inline void shader_set_mat2(Shader *shader, const char *name, const mat2s mat) {
  glUniformMatrix2fv(glGetUniformLocation(shader->ID, name), 1, GL_FALSE,
                     *mat.raw);
}
// ------------------------------------------------------------------------
inline void shader_set_mat3(Shader *shader, const char *name, const mat3s mat) {
  glUniformMatrix3fv(glGetUniformLocation(shader->ID, name), 1, GL_FALSE,
                     *mat.raw);
}
// ------------------------------------------------------------------------
inline void shader_set_mat4(Shader *shader, const char *name, const mat4s mat) {
  glUniformMatrix4fv(glGetUniformLocation(shader->ID, name), 1, GL_FALSE,
                     *mat.raw);
}

// Helpers
inline void check_shader_compilation(GLuint shader, const char *shader_type,
                                     const char *filename) {
  int success;
  char infoLog[512];
  glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
  if (!success) {
    glGetShaderInfoLog(shader, 512, NULL, infoLog);
    fprintf(stderr,
            "ERROR: %s shader compilation failed\nFile: %s\nDetails:\n%s\n",
            shader_type, filename, infoLog);
  }
}

inline void check_program_linking(GLuint programID) {
  int success;
  char infoLog[512];
  glGetProgramiv(programID, GL_LINK_STATUS, &success);
  if (!success) {
    glGetProgramInfoLog(programID, 512, NULL, infoLog);
    fprintf(stderr, "ERROR: Shader program linking failed %s\n", infoLog);
  }
}

#endif // SHADER_IMPLEMENTATION
