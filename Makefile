# Makefile for C program

# Compiler and flags
CC = gcc
CFLAGS = -ggdb -Wall -Wextra -std=c11 
CLINKS = -lglfw -lGLU -lGLEW -lGL -lglut -lm 

# Directories
SRC_DIR = .
BIN_DIR = ./bin
INC_DIR = -I./include/ -I./lib/

# gcc -o hellot.exe main.cpp glfw3dll.a libglew32.dll.a 

# Source file and output file
SOURCE = $(SRC_DIR)/main.c
OUTPUT = $(BIN_DIR)/a

# Targets
all: build

build: $(OUTPUT)

$(OUTPUT): $(SOURCE)
	@mkdir -p $(BIN_DIR)
	$(CC) $(CFLAGS) $(INC_DIR) -o $@ $< $(CLINKS)

run: build
	$(OUTPUT)

clean:
	rm -rf $(BIN_DIR)

.PHONY: all build run clean
