# Dynamic Racing Simulator

**Course:** Computer Animation

Dynamic Racing Simulator is a 2D car racing simulation that models multi-lane racing dynamics on a textured elliptical track. The project demonstrates principles of animation, motion control, and camera management in a simulated racing environment.

---

## Table of Contents

- [How It Works](#how-it-works)  
- [Animation Techniques](#animation-techniques)  
- [Topics Covered](#topics-covered)  
- [Features](#features)  

---

## How It Works

The simulator operates in a real-time update-render loop using OpenGL and GLUT:

1. **Track Generation:**
   - Three lanes are generated along an elliptical path.
   - Lane positions are calculated using parametric equations for ellipses.
   - Optional textures are applied for asphalt, grass, and curbs.

2. **Car Movement:**
   - Each car moves along a sequence of target points on its lane.
   - Cars maintain a `targetSpeed` and accelerate or decelerate smoothly using linear interpolation.
   - Lane-switching logic allows cars to avoid slower cars by checking neighboring lanes for available space.

3. **Collision Avoidance:**
   - Simple proximity detection prevents cars from overlapping.
   - Speed adjustment occurs dynamically when a car approaches another in the same lane.

4. **Camera Control:**
   - Three camera modes allow users to view the race dynamically:
     - Overview: Fixed view of the entire track.
     - Follow Leader: Smoothly follows the car furthest along the track.
     - Follow Specific Car: Focuses on a user-selected car.

5. **Sidebar Display:**
   - Real-time UI shows car names, current lap, and position.
   - Cars are sorted by progress to display accurate race standings.

6. **Rendering:**
   - Cars and track elements are drawn using OpenGL primitives (quads, lines) and optional textures.
   - Wheel rotation is animated based on car speed for visual realism.

---

## Animation Techniques

- **Keyframe-like Updates:**
  - Positions and rotations are updated incrementally each frame to create smooth motion.

- **Linear Interpolation (LERP):**
  - Used for acceleration, deceleration, lane-switch offsets, and camera movement to ensure smooth transitions.

- **Rotation Animation:**
  - Steering rotation is smoothed using angular interpolation to simulate realistic turning.

- **Texture Mapping:**
  - Textures are applied to track and cars for visual enhancement.

- **Real-time Rendering Loop:**
  - OpenGL double-buffering is used to avoid flickering and maintain smooth animation.

---

## Topics Covered

- 2D Kinematics and Motion Control  
- Linear Interpolation (LERP) for animation smoothing  
- Lane-switching algorithms and collision avoidance  
- Camera manipulation and view transforms in OpenGL  
- Rendering primitives and texture mapping in OpenGL  
- Event-driven programming with GLUT (keyboard and timer callbacks)  
- Real-time animation loop management  
- Basic UI overlay with text rendering  

---

## Features

- Multi-lane racing with AI-controlled cars  
- Dynamic camera modes for different views  
- Smooth acceleration, deceleration, and steering  
- Real-time position and lap display  
- Optional textured environment for enhanced visual appeal  
