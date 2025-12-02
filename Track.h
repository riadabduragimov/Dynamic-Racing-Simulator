#pragma once
#include "Vector2.h"
#include <vector>
#include <cmath>
#include <GL/glut.h>

// -------------------- Track Class --------------------
// Represents a basic racing track with three parallel lanes (inner, center, outer)
// Supports optional textures for visual enhancement: asphalt, grass, and curbs.
// Provides methods to draw the track layers and manages track geometry.
class Track {
public:
    // -------------------- Lane Geometry --------------------
    // Each lane is a sequence of 2D points representing the path of that lane
    std::vector<Vector2> lane1;  // Center lane
    std::vector<Vector2> lane2;  // Outer lane (offset from center)
    std::vector<Vector2> lane3;  // Inner lane (offset from center)

    // -------------------- Optional Textures --------------------
    // OpenGL texture IDs for rendering track surfaces
    GLuint asphaltTextureID;  // Texture for road surface
    GLuint grassTextureID;    // Texture for background/grass
    GLuint curbTextureID;     // Texture for track edges/curbs

    // -------------------- Track Radii --------------------
    // Horizontal (radiusX) and vertical (radiusY) radii of the track ellipse
    // Used to generate lane points and for simple boundary checks
    float radiusX;
    float radiusY;

    // -------------------- Constructor --------------------
    // Initializes track geometry (elliptical lanes) and attempts to load textures
    Track();

    // -------------------- Public Draw Method --------------------
    // Draws the full track in layers: grass, asphalt, curbs, and lane lines
    void draw();

private:
    // -------------------- Layered Drawing Methods --------------------
    // Draw the grass background (plain or textured)
    void drawGrass();

    // Draw the asphalt road surface (textured or plain fallback)
    void drawAsphalt();

    // Draw track curbs / lane boundaries as line loops
    void drawCurbs();

    // -------------------- Helper Method --------------------
    // Loads a BMP texture file using the global loader (loadBMP_custom)
    // Returns 0 if the file cannot be loaded
    GLuint loadTexture(const char* filename);
};

// -------------------- Global Track Radii --------------------
// File-scope global variables defined in Track.cpp
// Can be used by other modules (e.g., Car.cpp) for boundary checks or camera limits
extern float g_trackRadiusX;
extern float g_trackRadiusY;
