#pragma once
#include "Vector2.h"
#include <vector>

// -------------------- Car Class --------------------
// Represents a single car in the racing simulation.
// Handles movement, lane switching, acceleration, rotation, and rendering.
class Car {
public:
    // -------------------- Position & Movement --------------------
    Vector2 position;        // Current position of the car on the track
    Vector2 velocity;        // Current velocity vector of the car
    float speed;             // Current speed magnitude
    float targetSpeed;       // Desired speed considering traffic and acceleration
    float maxSpeed;          // Maximum achievable speed
    float accelerationFactor;// Factor controlling acceleration responsiveness
    float acceleration;      // Current frame acceleration applied
    float size;              // Half-size of the car for rendering and collision checks

    // -------------------- Appearance --------------------
    int colorR, colorG, colorB; // RGB color values for car body (0-255)
    float rotation;             // Car orientation in degrees
    float wheelRotation;        // Visual rotation of wheels for animation

    // -------------------- Track Progress --------------------
    int targetIndex;           // Index of the next track point to reach
    int lap;                   // Current lap number
    std::vector<Vector2>* trackPoints; // Pointer to the vector of track points
    bool finished;             // Flag indicating if the car has finished the race

    // -------------------- Lane Management --------------------
    std::vector<std::vector<Vector2>>* allLanes; // Pointer to all lanes on track
    int laneIndex;            // Current lane index the car occupies
    Vector2 laneOffset;       // Lateral offset for smooth lane switching
    int targetLaneIndex;      // Lane index the car is trying to switch to

    // -------------------- Steering & Control --------------------
    float laneSwitchSpeed;    // Speed factor for lateral lane movement
    float rotationSpeed;      // Speed factor for smooth rotation adjustment
    float steerAngle;         // Current steering angle for visual or physics purposes
    float slipFactor;         // Factor for simulating car slipping during turns

    // -------------------- Constructor --------------------
    // Initializes a car with position, color, track information, and initial lane
    Car(Vector2 pos, float r, float g, float b, std::vector<Vector2>* track,
        std::vector<std::vector<Vector2>>* lanes, int lane);

    // -------------------- Simulation Functions --------------------
    // Updates the car's speed, position, rotation, lane switching, and lap progress
    void update(float dt, std::vector<Car>* cars);

    // Renders the car body and wheels at the current position and rotation
    void draw();
};
