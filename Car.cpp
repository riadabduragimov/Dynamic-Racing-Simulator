#include "stdafx.h"
#include "Car.h"
#include <GL/glut.h>
#include <cmath>
#include <algorithm>

// Define PI if not already defined
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

// -------------------- Constructor --------------------
// Initializes a Car object with position, color, track info, and lane assignment
Car::Car(Vector2 pos, float r, float g, float b, std::vector<Vector2>* track,
    std::vector<std::vector<Vector2>>* lanes, int lane)
    : position(pos), speed(0.0f), targetSpeed(2.0f), maxSpeed(5.0f), accelerationFactor(2.0f),
    size(0.5f), targetIndex(1), lap(0), trackPoints(track), finished(false),
    rotation(0.0f), wheelRotation(0.0f), allLanes(lanes), laneIndex(lane), targetLaneIndex(lane),
    laneSwitchSpeed(3.0f), rotationSpeed(5.0f), steerAngle(0.0f), slipFactor(0.1f)
{
    // Convert RGB to 0-255 range
    colorR = r * 255;
    colorG = g * 255;
    colorB = b * 255;

    laneOffset = Vector2(0, 0); // Initial lateral offset for smooth lane changes
}

// -------------------- Update Function --------------------
// Updates the car's position, speed, rotation, lane switching, and wheel rotation each frame
void Car::update(float dt, std::vector<Car>* cars) {
    if (finished) return; // Skip update if car has finished race

    // Calculate vector to next target track point
    Vector2 target = trackPoints->at(targetIndex);
    Vector2 dir = target - position;
    float dist = dir.length();
    float minDist = size * 2.0f; // Minimum distance to detect other cars

    // -------------------- Front Car Detection --------------------
    Car* frontCar = nullptr;
    for (auto& c : *cars) {
        if (&c == this) continue;
        // Check for car in the same lane ahead within minimum distance
        if (c.laneIndex == laneIndex && c.targetIndex >= targetIndex &&
            (c.position - position).length() < minDist * 2.0f) {
            frontCar = &c;
            break;
        }
    }

    // -------------------- Target Speed Calculation --------------------
    targetSpeed = maxSpeed;
    if (frontCar) targetSpeed = std::max(0.5f, frontCar->speed - 0.5f); // Avoid collision

    // -------------------- Lane Switching Logic --------------------
    if (frontCar && (frontCar->position - position).length() < minDist * 1.5f) {
        int newLane = -1;
        if (laneIndex > 0) newLane = laneIndex - 1; // Check left lane
        else if (laneIndex < allLanes->size() - 1) newLane = laneIndex + 1; // Check right lane

        // Ensure new lane is free of other cars
        if (newLane >= 0) {
            bool spaceFree = true;
            for (auto& c : *cars) {
                if (&c != this && c.laneIndex == newLane &&
                    (c.position - position).length() < minDist) {
                    spaceFree = false;
                    break;
                }
            }
            if (spaceFree) targetLaneIndex = newLane; // Set new lane target
        }
    }

    // -------------------- Smooth Acceleration --------------------
    speed += (targetSpeed - speed) * dt * accelerationFactor;
    if (speed < 0) speed = 0;

    // -------------------- Smooth Lane Offset --------------------
    Vector2 desiredOffset = Vector2(0, (targetLaneIndex - laneIndex) * 1.5f);
    laneOffset += (desiredOffset - laneOffset) * dt * laneSwitchSpeed;

    // -------------------- Steering & Rotation --------------------
    if (dist > 0.01f) {
        Vector2 moveDir = dir.normalized();
        float desiredRotation = atan2(moveDir.y, moveDir.x) * 180.0f / M_PI;
        float rotationDiff = desiredRotation - rotation;

        // Keep rotation difference between -180 to 180
        if (rotationDiff > 180) rotationDiff -= 360;
        if (rotationDiff < -180) rotationDiff += 360;

        rotation += rotationDiff * dt * rotationSpeed; // Smoothly rotate car
    }

    // -------------------- Position Update --------------------
    if (dist > 0.01f)
        position += dir.normalized() * speed * dt + laneOffset * dt;

    // -------------------- Wheel Rotation --------------------
    wheelRotation += speed * dt * 360.0f / (2.0f * M_PI * size); // Rotate wheels based on speed

    // -------------------- Track Progress --------------------
    if (dist < 0.1f) {
        targetIndex++;
        if (targetIndex >= trackPoints->size()) {
            lap++;          // Increment lap
            targetIndex = 0; // Loop track
        }
    }

    // Update lane index after possible lane change
    laneIndex = targetLaneIndex;
}

// -------------------- Draw Function --------------------
// Draws the car as a colored rectangle with simple wheels, applying rotation and lane offset
void Car::draw() {
    if (finished) return; // Skip drawing finished cars

    glPushMatrix();
    glTranslatef(position.x, position.y, 0.0f); // Move to car position
    glRotatef(rotation, 0.0f, 0.0f, 1.0f);      // Rotate car according to movement direction

    // -------------------- Draw Car Body --------------------
    glColor3ub(colorR, colorG, colorB); // Set car color
    glBegin(GL_QUADS);
    glVertex2f(-size, -size);
    glVertex2f(size, -size);
    glVertex2f(size, size);
    glVertex2f(-size, size);
    glEnd();

    // -------------------- Draw Wheels --------------------
    float wheelSize = size / 3.0f;

    // Left wheel
    glPushMatrix();
    glTranslatef(-size + wheelSize, -size, 0);
    glRotatef(wheelRotation, 0, 0, 1);
    glColor3ub(0, 0, 0); // Black wheels
    glBegin(GL_QUADS);
    glVertex2f(-wheelSize, -wheelSize);
    glVertex2f(wheelSize, -wheelSize);
    glVertex2f(wheelSize, wheelSize);
    glVertex2f(-wheelSize, wheelSize);
    glEnd();
    glPopMatrix();

    // Right wheel
    glPushMatrix();
    glTranslatef(size - wheelSize, -size, 0);
    glRotatef(wheelRotation, 0, 0, 1);
    glColor3ub(0, 0, 0);
    glBegin(GL_QUADS);
    glVertex2f(-wheelSize, -wheelSize);
    glVertex2f(wheelSize, -wheelSize);
    glVertex2f(wheelSize, wheelSize);
    glVertex2f(-wheelSize, wheelSize);
    glEnd();
    glPopMatrix();

    glPopMatrix(); // Restore transform
}
