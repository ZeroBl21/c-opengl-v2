#version 330 core

in vec3 ourColor;
// in vec3 ourPosition;

out vec4 FragColor;

void main () {
  FragColor = vec4(ourColor, 1.0);
  // FragColor = vec4(ourPosition, 1.0);
}
