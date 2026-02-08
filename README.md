# Boids Simulation Engine

A hybrid simulation engine demonstrating emergent behavior in autonomous agents (flocking). 
The project decouples physics calculations from rendering using a high-performance IPC architecture.

<div align="center">
  <img src="Images/Image.gif" width="600" title="Boids Simulation">
</div>

## Architecture & Design Patterns

This is not just a script, but a distributed system simulation running in two distinct processes:

1.  **Backend (C++ Engine):** Handles all physics calculations, vector math, and Reynolds' rules.
    * Optimized for performance to handle $O(N^2)$ complexity per frame.
    * Implements "Sticky Walls" handling and velocity clamping.
2.  **Frontend (Python Visualization):**
    * Acts as a pure renderer.
    * Decouples the frame rate of the simulation from the visualization loop.
3.  **Communication Layer (ZeroMQ):**
    * Uses a Publisher-Subscriber (PUB-SUB) pattern.
    * Transmits serialized agent data (Position/Velocity) via TCP sockets in real-time.

## Key Features

* **Emergent Behavior:** Implements Craig Reynolds' three flocking steering behaviors:
    * **Separation:** Avoid crowding local flockmates.
    * **Alignment:** Steer towards the average heading of local flockmates.
    * **Cohesion:** Steer to move towards the average position (center of mass) of local flockmates.
* **Hybrid Language Stack:** Leveraging C++ for raw speed and Python for rapid UI prototyping.
* **Memory Safety:** Strict object lifecycle management in C++ (avoiding unnecessary copies).

## Tech Stack

* **Core Logic:** C++17 (STL, Vector Math)
* **Visualization:** Python 3 (PyGame)
* **Networking:** ZeroMQ (libzmq, pyzmq)
* **Build System:** CMake

## How to Run

### Prerequisites
* C++ Compiler (GCC/Clang/MSVC)
* Python 3.x
* ZeroMQ Library

### Installation

1.  **Clone the repository**
    ```bash
    git clone [https://github.com/YOUR_USERNAME/boids-simulation.git](https://github.com/YOUR_USERNAME/boids-simulation.git)
    cd boids-simulation
    ```

2.  **Build the C++ Engine**
    ```bash
    cd engine
    mkdir build && cd build
    cmake ..
    make
    # Run the engine (it will wait for a listener)
    ./boids_engine
    ```

3.  **Run the Visualization (in a separate terminal)**
    ```bash
    cd backend
    pip install pyzmq
    python app.py
    ```

## Future Improvements
* Spatial Partitioning (Quadtree) to optimize from $O(N^2)$ to $O(N \log N)$.
* Interactive obstacles added via mouse input.
* Predator agent implementation.

---
