#define ARENA_IMPLEMENTATION
#include "arena.h"

#include "str.h"

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <wchar.h>

static string read_file(const char *filename, arena *a) {
  FILE *file = fopen(filename, "rb");
  if (!file) {
    string empty_str = {0}; // Return an empty string if file cannot be opened
    fprintf(stderr, "Error: No se pudo abrir el archivo %s\n", filename);
    return empty_str;
  }

  fseek(file, 0, SEEK_END); // Seek to the end of the file
  long file_size =
      ftell(file); // Get the current position, which represents the file size
  fseek(file, 0, SEEK_SET); // Seek back to the beginning of the file

  // Allocate memory in the arena for the file contents
  uint8_t *file_contents =
      make(a, uint8_t, file_size + 1); // Plus 1 for null terminator
  if (!file_contents) {
    fclose(file); // Close the file if memory allocation fails
    fprintf(
        stderr,
        "Error: No se pudo asignar memoria para el contenido del archivo %s\n",
        filename);
    string empty_str = {0};
    return empty_str;
  }

  // Read the file contents into memory
  size_t bytes_read = fread(file_contents, 1, file_size, file);
  if (bytes_read != (size_t)file_size) {
    fclose(file); // Close the file if read fails
    fprintf(stderr, "Error: No se pudo leer correctamente el archivo %s\n",
            filename);
    string empty_str = {0};
    return empty_str;
  }
  file_contents[file_size] = '\0'; // Null-terminate the string

  fclose(file); // Close the file after reading

  // Create and return the str structure containing the file contents
  string file_str = {.data = file_contents, .len = file_size};
  return file_str;
}
