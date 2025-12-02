#pragma once
#include <GL/glut.h>

// -------------------- Custom BMP Loader --------------------
// Loads a 24-bit uncompressed BMP image from disk and creates an OpenGL texture.
// The function returns the OpenGL texture ID on success, or 0 if the file could
// not be loaded or is invalid. This loader does not support BMP files with color palettes.
GLuint loadBMP_custom(const char* imagepath);

// -------------------- Global Texture IDs --------------------
// OpenGL texture IDs for car and wheel graphics. These are initialized in
// Textures.cpp after loading the respective BMP images.
extern GLuint g_carTex;     // Main car texture
extern GLuint g_wheelTex;   // Wheel texture
