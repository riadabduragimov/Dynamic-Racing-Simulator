#include "stdafx.h"
#include "Car.h"
#include "Track.h"
#include "Textures.h"
#include <vector>
#include <GL/glut.h>
#include <string>
#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <algorithm>

// Camera globals
static float camX = 0.0f;        // Camera X position
static float camY = 0.0f;        // Camera Y position
static float camZoom = 1.0f;     // Camera zoom level
static int cameraMode = 0;       // Camera mode: 0 = overview, 1 = follow leading car, 2 = follow specific car
static int followCarIndex = 0;   // Index of car to follow in mode 2

// Track and cars
Track track;                     // Track object
std::vector<Car> cars;           // Vector storing all cars
std::vector<std::string> carNames = { "BMW", "Mercedes", "Ford" }; // Names of the cars

// -------------------- Utility Functions --------------------

// Display text in world coordinates at (x, y)
void displayText(float x, float y, const std::string& text) {
    glRasterPos2f(x, y);
    for (char c : text)
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, c);
}

// Display text in screen coordinates at (x, y) regardless of camera transform
void displayScreenText(float x, float y, const std::string& text) {
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    gluOrtho2D(0, 900, 0, 700); // Screen space coordinates
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();

    glColor3f(1.0f, 1.0f, 1.0f); // White text
    glRasterPos2f(x, y);
    for (char c : text)
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, c);

    glPopMatrix();
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
}

// Draw sidebar showing the positions of all cars
void drawSidebar() {
    glDisable(GL_TEXTURE_2D);  // Disable textures for sidebar
    glColor3f(0.1f, 0.1f, 0.1f); // Dark background
    glBegin(GL_QUADS);
    glVertex2f(700, 0);
    glVertex2f(900, 0);
    glVertex2f(900, 700);
    glVertex2f(700, 700);
    glEnd();
    glEnable(GL_TEXTURE_2D); // Re-enable textures

    // Calculate progress for each car
    std::vector<std::pair<int, float>> carProgress; // <carIndex, distanceAlongTrack>
    for (int i = 0; i < cars.size(); ++i) {
        float progress = cars[i].lap * track.lane1.size() + cars[i].targetIndex;
        carProgress.push_back({ i, progress });
    }

    // Sort cars in descending order of progress
    std::sort(carProgress.begin(), carProgress.end(), [](auto& a, auto& b) { return a.second > b.second; });

    // Display car positions and lap number
    for (int i = 0; i < carProgress.size(); ++i) {
        int idx = carProgress[i].first;
        Car& c = cars[idx];
        std::string info = std::to_string(i + 1) + ". " + carNames[idx] + " Lap: " + std::to_string(c.lap);
        displayScreenText(710, 670 - i * 30, info);
    }
}

// -------------------- Rendering --------------------

// Main display callback
void display() {
    glClear(GL_COLOR_BUFFER_BIT);
    glLoadIdentity();

    // Apply camera transformations
    glPushMatrix();
    glScalef(camZoom, camZoom, 1.0f);
    glTranslatef(-camX, -camY, 0.0f);

    // Draw the track
    track.draw();

    // Draw all cars and their lap info
    for (auto& car : cars) {
        car.draw();
        displayText(car.position.x - 0.3f, car.position.y + 1.0f, "Lap: " + std::to_string(car.lap));
    }

    glPopMatrix();

    // Draw sidebar overlay
    drawSidebar();

    glutSwapBuffers();
}

// -------------------- Update Loop --------------------

// Timer/update callback (~60 FPS)
void update(int value) {
    float dt = 0.016f; // Time step ~16ms

    // Update all cars
    for (auto& car : cars)
        car.update(dt, &cars);

    // Camera movement logic
    if (cameraMode == 1) {
        // Follow the leading car
        float leadX = 0.0f, leadY = 0.0f;
        float farthestDist = -1.0f;
        for (auto& car : cars) {
            float distAlongTrack = car.lap * track.lane1.size() + car.targetIndex;
            if (distAlongTrack > farthestDist) {
                farthestDist = distAlongTrack;
                leadX = car.position.x;
                leadY = car.position.y;
            }
        }
        camX += (leadX - camX) * 0.05f;
        camY += (leadY - camY) * 0.05f;
    }
    else if (cameraMode == 2 && followCarIndex >= 0 && followCarIndex < cars.size()) {
        // Follow a specific car
        Car& c = cars[followCarIndex];
        camX += (c.position.x - camX) * 0.05f;
        camY += (c.position.y - camY) * 0.05f;
    }
    else {
        // Overview mode: center camera on track
        camX += (0.0f - camX) * 0.05f;
        camY += (0.0f - camY) * 0.05f;
    }

    glutPostRedisplay();
    glutTimerFunc(16, update, 0);
}

// -------------------- Window/Projection --------------------

// Window resize callback
void reshape(int w, int h) {
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    // Set orthographic view based on track size
    float viewMargin = 2.0f;
    float maxX = 12.0f;
    float maxY = 7.0f;
    gluOrtho2D(-maxX - viewMargin, maxX + viewMargin, -maxY - viewMargin, maxY + viewMargin);

    glMatrixMode(GL_MODELVIEW);
}

// -------------------- Input Handling --------------------

// Keyboard input callback
void keyboard(unsigned char key, int x, int y) {
    if (key == '+') camZoom *= 1.1f;    // Zoom in
    else if (key == '-') camZoom /= 1.1f; // Zoom out
    else if (key == 'c' || key == 'C') cameraMode = (cameraMode + 1) % 3; // Toggle camera modes
    else if (key >= '1' && key <= '3') {
        // Follow a specific car
        int carNum = key - '1';
        if (carNum < cars.size()) {
            followCarIndex = carNum;
            cameraMode = 2; // Switch to specific car mode
            printf("Now following: %s\n", carNames[followCarIndex].c_str());
        }
    }
    else if (key == 27) exit(0); // ESC key exits
    glutPostRedisplay();
}

// Special keys input callback (arrow keys)
void specialKeys(int key, int x, int y) {
    float pan = 0.5f / camZoom;
    if (key == GLUT_KEY_LEFT) camX -= pan;
    if (key == GLUT_KEY_RIGHT) camX += pan;
    if (key == GLUT_KEY_UP) camY += pan;
    if (key == GLUT_KEY_DOWN) camY -= pan;
    glutPostRedisplay();
}

// -------------------- Main Program --------------------
int main(int argc, char** argv) {
    srand(static_cast<unsigned int>(time(nullptr))); // Seed RNG

    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowSize(900, 700);
    glutCreateWindow("Car Racing Track Simulation");

    // Enable alpha blending & textures
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_TEXTURE_2D);

    // Load textures
    g_carTex = loadBMP_custom("car.bmp");
    g_wheelTex = loadBMP_custom("wheel.bmp");

    if (!g_carTex) printf("Warning: car.bmp not loaded (falling back to color)\n");
    if (!g_wheelTex) printf("Warning: wheel.bmp not loaded (falling back to simple wheels)\n");

    // Build lane data
    std::vector<std::vector<Vector2>> allLanes = { track.lane1, track.lane2, track.lane3 };

    // Initialize cars with lane, color, speed, and acceleration
    for (int i = 0; i < 3; ++i) {
        // Assign distinct RGB color for each car
        float r = (i == 0) ? 1.0f : 0.0f; // BMW red
        float g = (i == 1) ? 1.0f : 0.0f; // Mercedes green
        float b = (i == 2) ? 1.0f : 0.0f; // Ford blue
        std::vector<Vector2>* lane = &allLanes[i];

        Car car(lane->at(0), r, g, b, lane, &allLanes, i);
        car.maxSpeed = 4.8f + static_cast<float>(rand()) / RAND_MAX * 0.5f;
        car.speed = 1.0f + static_cast<float>(rand()) / RAND_MAX;
        car.accelerationFactor = 1.8f + static_cast<float>(rand()) / RAND_MAX * 0.5f;
        cars.push_back(car);
    }

    // Register callbacks
    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutKeyboardFunc(keyboard);
    glutSpecialFunc(specialKeys);
    glutTimerFunc(16, update, 0);

    // Enter main loop
    glutMainLoop();
    return 0;
}

