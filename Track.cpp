#include "stdafx.h"
#include "Track.h"
#include "Textures.h" // Provides loadBMP_custom for texture loading
#include <GL/glut.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

// -------------------- Global Track Radii --------------------
// These global variables are used elsewhere for track boundary checks, camera limits, etc.
float g_trackRadiusX = 10.0f;
float g_trackRadiusY = 5.0f;

// -------------------- Texture Loading Helper --------------------
// Loads a BMP texture file and returns the OpenGL texture ID
// Returns 0 if the file is missing, allowing fallback rendering
GLuint Track::loadTexture(const char* filename) {
    return loadBMP_custom(filename);
}

// -------------------- Track Constructor --------------------
// Initializes track geometry (elliptical lanes) and optional textures
Track::Track()
    : asphaltTextureID(0), grassTextureID(0), curbTextureID(0),
    radiusX(10.0f), radiusY(5.0f)
{
    const int points = 200;           // Number of points used to approximate the track curve
    const float radiusX_local = radiusX; // Horizontal radius of ellipse
    const float radiusY_local = radiusY; // Vertical radius of ellipse
    const float laneSpacing = 1.5f;   // Lateral offset between lanes

    // Generate points for 3 lanes along an elliptical track
    for (int i = 0; i < points; ++i) {
        float t = (2.0f * M_PI * i) / points; // Angle in radians around the ellipse
        float x = radiusX_local * cos(t);     // X coordinate along ellipse
        float y = radiusY_local * sin(t);     // Y coordinate along ellipse

        lane1.push_back(Vector2(x, y));          // Center lane
        lane2.push_back(Vector2(x, y + laneSpacing)); // Outer lane
        lane3.push_back(Vector2(x, y - laneSpacing)); // Inner lane
    }

    // Update global radii for camera or collision checks
    g_trackRadiusX = radiusX_local;
    g_trackRadiusY = radiusY_local;

    // Attempt to load optional textures from executable folder
    asphaltTextureID = loadTexture("asphalt.bmp"); // Main road surface
    grassTextureID = loadTexture("grass.bmp");     // Background
    curbTextureID = loadTexture("curb.bmp");       // Track edges/curbs
}

// -------------------- Draw Grass --------------------
// Renders the track background, either textured or fallback plain green
void Track::drawGrass() {
    if (!grassTextureID) {
        // Fallback plain grass
        glDisable(GL_TEXTURE_2D);
        glColor3f(0.85f, 0.95f, 0.85f);
        glBegin(GL_QUADS);
        glVertex2f(-40.0f, -40.0f);
        glVertex2f(40.0f, -40.0f);
        glVertex2f(40.0f, 40.0f);
        glVertex2f(-40.0f, 40.0f);
        glEnd();
        glEnable(GL_TEXTURE_2D);
        return;
    }

    // Optional: draw textured grass (currently using plain color)
    glDisable(GL_TEXTURE_2D);
    glColor3f(0.85f, 0.95f, 0.85f);
    glBegin(GL_QUADS);
    glVertex2f(-40.0f, -40.0f);
    glVertex2f(40.0f, -40.0f);
    glVertex2f(40.0f, 40.0f);
    glVertex2f(-40.0f, 40.0f);
    glEnd();
    glEnable(GL_TEXTURE_2D);
}

// -------------------- Draw Asphalt --------------------
// Draws the road surface using a texture if available, otherwise fallback plain gray
void Track::drawAsphalt() {
    if (asphaltTextureID) {
        // Bind texture and draw quad strip between inner and outer lane boundaries
        glBindTexture(GL_TEXTURE_2D, asphaltTextureID);
        glColor3ub(255, 255, 255);
        glBegin(GL_QUAD_STRIP);

        int n = static_cast<int>(lane2.size());
        for (int i = 0; i <= n; ++i) {
            int idx = i % n;                  // Wrap around at the end
            Vector2 pOuter = lane3[idx];      // Outer lane point
            Vector2 pInner = lane1[idx];      // Inner lane point
            float t = i / (float)n;           // Texture coordinate along track
            glTexCoord2f(t * 4.0f, 0.0f); glVertex2f(pOuter.x, pOuter.y);
            glTexCoord2f(t * 4.0f, 1.0f); glVertex2f(pInner.x, pInner.y);
        }
        glEnd();
        glBindTexture(GL_TEXTURE_2D, 0);
    }
    else {
        // Fallback plain road if texture missing
        glDisable(GL_TEXTURE_2D);
        glColor3f(0.2f, 0.2f, 0.2f);
        glBegin(GL_QUAD_STRIP);

        int n = static_cast<int>(lane2.size());
        for (int i = 0; i <= n; ++i) {
            int idx = i % n;
            Vector2 pOuter = lane3[idx];
            Vector2 pInner = lane1[idx];
            glVertex2f(pOuter.x, pOuter.y);
            glVertex2f(pInner.x, pInner.y);
        }
        glEnd();
        glEnable(GL_TEXTURE_2D);
    }
}

// -------------------- Draw Curbs --------------------
// Draws simple curbs/edges as thin line loops along inner and outer lane boundaries
void Track::drawCurbs() {
    glDisable(GL_TEXTURE_2D);
    glLineWidth(2.0f);
    glColor3f(0.9f, 0.9f, 0.9f);

    // Inner boundary (lane1)
    glBegin(GL_LINE_LOOP);
    for (auto& p : lane1) glVertex2f(p.x, p.y);
    glEnd();

    // Outer boundary (lane3)
    glBegin(GL_LINE_LOOP);
    for (auto& p : lane3) glVertex2f(p.x, p.y);
    glEnd();

    glEnable(GL_TEXTURE_2D);
}

// -------------------- Draw Complete Track --------------------
// Renders the full track in layers: grass, asphalt, curbs, and lane lines
void Track::draw() {
    drawGrass();     // Background
    drawAsphalt();   // Road surface
    drawCurbs();     // Track edges

    // Draw lane lines for all three lanes
    glLineWidth(3.0f);
    glDisable(GL_TEXTURE_2D);
    glColor3f(0.1f, 0.1f, 0.1f);

    // Lambda helper to draw a lane as a line loop
    auto drawLane = [](std::vector<Vector2>& lane) {
        glBegin(GL_LINE_LOOP);
        for (auto& p : lane) glVertex2f(p.x, p.y);
        glEnd();
        };

    drawLane(lane1);
    drawLane(lane2);
    drawLane(lane3);

    glEnable(GL_TEXTURE_2D);
}
