#include "stdafx.h"
#include "Textures.h"
#include <cstdio>
#include <cstdlib>

// -------------------- Global Texture IDs --------------------
// OpenGL texture IDs for cars and wheels, initialized to 0 (not loaded yet)
GLuint g_carTex = 0;    // Main car texture
GLuint g_wheelTex = 0;  // Wheel texture

// -------------------- Custom BMP Loader --------------------
// Simple 24-bit BMP loader (no color palette support).
// Loads a BMP file from disk, converts BGR to RGB, and uploads as an OpenGL texture.
// Returns the generated OpenGL texture ID, or 0 on failure.
GLuint loadBMP_custom(const char* imagepath) {
    // Open file in binary mode
    FILE* file = fopen(imagepath, "rb");
    if (!file) return 0;  // Failed to open file

    // Read BMP header (54 bytes)
    unsigned char header[54];
    if (fread(header, 1, 54, file) != 54) { fclose(file); return 0; }

    // Verify BMP signature ('B' 'M')
    if (header[0] != 'B' || header[1] != 'M') { fclose(file); return 0; }

    // Extract image metadata from header
    unsigned int dataPos = *(int*)&(header[0x0A]);  // Start of pixel data
    unsigned int imageSize = *(int*)&(header[0x22]);  // Size of raw image data
    unsigned int width = *(int*)&(header[0x12]);  // Image width in pixels
    unsigned int height = *(int*)&(header[0x16]);  // Image height in pixels

    // Default values if not set in BMP header
    if (imageSize == 0) dataPos = width * height * 3;
    if (dataPos == 0) dataPos = 54;

    // Allocate buffer and read pixel data
    unsigned char* data = (unsigned char*)malloc(imageSize);
    fseek(file, dataPos, SEEK_SET);
    fread(data, 1, imageSize, file);
    fclose(file);

    // BMP stores pixels in BGR order; convert to RGB
    for (unsigned int i = 0; i < imageSize; i += 3) {
        unsigned char tmp = data[i];
        data[i] = data[i + 2];
        data[i + 2] = tmp;
    }

    // Generate and bind OpenGL texture
    GLuint textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);

    // Set texture filtering parameters (linear)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // Upload pixel data to GPU
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);

    // Free CPU-side memory
    free(data);

    // Return OpenGL texture ID
    return textureID;
}
