# LumaSort Engine

> **Copyright (c) 2025**  
> This project is licensed under the **CC BY-NC-ND 4.0** license.  
> See the [LICENSE](LICENSE) file for details.

[![C++](https://img.shields.io/badge/C%2B%2B-20-00599C?style=for-the-badge&logo=c%2B%2B&logoColor=white)](https://isocpp.org/)
[![OpenGL](https://img.shields.io/badge/OpenGL-3.3-FFFFFF?style=for-the-badge&logo=opengl)](https://www.opengl.org/)
[![Dear ImGui](https://img.shields.io/badge/Dear_ImGui-Docking-880000?style=for-the-badge&logo=c%2B%2B&logoColor=white)](https://github.com/ocornut/imgui)
[![OpenCV](https://img.shields.io/badge/OpenCV-4.x-5C3EE8?style=for-the-badge&logo=opencv&logoColor=white)](https://opencv.org/)
[![CMake](https://img.shields.io/badge/CMake-Build-064F8C?style=for-the-badge&logo=cmake&logoColor=white)](https://cmake.org/)
[![vcpkg](https://img.shields.io/badge/vcpkg-Manifest-purple?style=for-the-badge&logo=c%2B%2B&logoColor=white)](https://vcpkg.io/)

---

## Introduction

**LumaSort Engine** is a high-performance C++20 real-time visualizer that rearranges pixels from live video feeds, static images, or real-time drawings into a target image based on global luminance intensity. By combining a "Pixel Sorting" algorithm with Fluid Dynamics (Flow Fields), it creates a mesmerizing, fluid-like transition of pixels finding their new "home" based on brightness.

This engine is built for performance, leveraging modern OpenGL for rendering, OpenCV for computer vision input, and a robust C++20 architecture required for real-time particle simulations.

---

## Table of Contents

- [Introduction](#introduction)
- [How It Works](#how-it-works)
- [Architecture](#architecture)
- [Tech Stack](#tech-stack)
- [Directory Structure](#directory-structure)
- [Setup & Quick Start](#setup--quick-start)
- [Features](#features)
- [Troubleshooting](#troubleshooting)
- [License](#license)

---

## How It Works

1.  **Input Analysis**: The engine captures frames from a webcam or loads an image via OpenCV.
2.  **Luminance Mapping**: Every pixel in the source is analyzed for its luminance (brightness) value.
3.  **Target Matching**: A target image (e.g., a portrait) is pre-processed to understand the luminance distribution of its pixels.
4.  **Particle Simulation**: Millions of particles (pixels) are spawned. Instead of instantly snapping to the target, they flow towards their destination using a Flow Field (Curl Noise or Perlin Noise), creating a fluid motion effect.
5.  **Sorting**: The core sorting algorithm ensures that a bright pixel from the webcam eventually settles into a bright spot on the target image.

---

## Architecture

The engine follows a strict separation of concerns, dividing the application into Logic (Math/Sort), Data (app state), and Presentation (Renderer/UI).

```mermaid
graph TD
    subgraph Core
        App[App Loop]
        Main[Main Entry] --> App
    end

    subgraph "Sub-systems"
        UI[GuiLayer (ImGui)]
        Gfx[Renderer (OpenGL)]
        Sorter[Sorter & FlowField]
    end

    subgraph Dependencies
        GLFW[GLFW Window]
        CV[OpenCV Input]
    end

    App -->|Updates| Sorter
    App -->|Renders| Gfx
    App -->|Renders| UI
    
    Sorter -->|Particles| Gfx
    CV -->|Frames| Sorter
    
    UI -- Controls --> Sorter
    UI -- Controls --> Gfx
    
    style App fill:#f9f,stroke:#333,color:#000
    style Sorter fill:#9cf,stroke:#333,color:#000
    style Gfx fill:#9f9,stroke:#333,color:#000
```

---

## Tech Stack

| Component | Technology | Reasoning |
| :--- | :--- | :--- |
| **Language** | C++20 | Required for high-performance memory management and modern language features (concepts, modules support). |
| **Build System** | CMake | Industry standard for cross-platform C++ build configuration. |
| **Package Manager** | vcpkg | Seamless integration of libraries in "Manifest Mode" ensures reproducible builds. |
| **Computer Vision** | OpenCV 4 | robust library for reading webcam feeds and efficient image matrix (`cv::Mat`) manipulation. |
| **Rendering** | OpenGL 3.3+ | Low-level hardware acceleration is necessary to render millions of particles at 60 FPS. |
| **Windowing** | GLFW + GLAD | Lightweight, standard way to create contexts and handle input on Linux/Windows. |
| **UI** | Dear ImGui | Immediate Mode GUI is perfect for real-time debugging and parameter tuning variables like flow speed. |
| **Math** | GLM | Standard mathematics library for graphics software (vectors, matrices). |

---

## Directory Structure

```text
lumasort-engine/
├── vcpkg.json              # Dependency Manifest
├── CMakeLists.txt          # Build Configuration
├── src/
│   ├── main.cpp            # Entry Point
│   ├── app.h/cpp           # Application & Event Loop
│   ├── core/
│   │   ├── sorter.h/cpp    # Sorting Logic (TBD)
│   │   ├── flow_field.h    # Fluid Math (TBD)
│   ├── graphics/
│   │   ├── renderer.h/cpp  # OpenGL Wrapper
│   └── ui/
│       ├── gui_layer.h/cpp # ImGui Overlay
├── assets/                 # Shaders and Images
└── build/                  # (Generated) Build artifacts
```

---

## Setup & Quick Start

### 1. Requirements
This project uses `vcpkg` which builds dependencies from source. This process requires a specific set of system development tools.

**Essential Build Tools:**
- C++ Compiler (GCC 11+ / Clang 14+)
- CMake (3.20+)
- Ninja or Make

**System Libraries (Linux):**
- **X11 / OpenGL**: `libx11-dev`, `libglu1-mesa-dev`
- **Build Helpers**: `bison`, `flex`, `gperf`, `pkg-config`
- **Python Build Environment**: `python3-venv` (Critical for building `libsystemd`)

### 2. Installation (One-line)
Run this command to ensure your system is ready:
```bash
sudo apt update && sudo apt install -y build-essential pkg-config cmake ninja-build autoconf autoconf-archive automake libtool bison flex gperf libx11-dev libxext-dev libxrandr-dev libxinerama-dev libxcursor-dev libxi-dev libglu1-mesa-dev libgl1-mesa-dev python3-venv python3-jinja2
```

### 3. Build & Run

1.  **Clone the Repository**
    ```bash
    git clone https://github.com/your-username/LumaSort-Engine.git
    cd LumaSort-Engine
    ```

2.  **Configure (CMake + vcpkg)**
    ```bash
    cmake -B build -S . -DCMAKE_TOOLCHAIN_FILE=/path/to/vcpkg/scripts/buildsystems/vcpkg.cmake
    ```
    *(Note: The first time you run this, vcpkg will download and build all dependencies. This may take 5-10 minutes.)*

3.  **Compile**
    ```bash
    cmake --build build
    ```

4.  **Run**
    ```bash
    ./build/LumaSort
    ```

---

## Troubleshooting

### `vcpkg install failed` with `python3 -m venv failed`
**Cause:** Older or minimal Linux distributions (like Ubuntu Server) often split the Python standard library. The `venv` module, required by the `libsystemd` build script, is missing.
**Fix:** Install the missing python module:
```bash
sudo apt install python3-venv
```

### `Could not find bison` or `GPERF not found`
**Cause:** Some dependencies (OpenCV, glib) generate code during their build process and require these generator tools.
**Fix:** Install the build tool suite:
```bash
sudo apt install bison flex gperf
```

### `CMake Error: CMAKE_MAKE_PROGRAM is not set`
**Cause:** You are missing a build backend like Make or Ninja.
**Fix:**
```bash
sudo apt install build-essential ninja-build
```

---

## License

This project is distributed under the **Creative Commons Attribution-NonCommercial-NoDerivatives 4.0 International** license.

**What this means:**

✅ You can view and use this code for learning  
✅ You can share this project with attribution  
❌ You cannot use this commercially  
❌ You cannot create modified versions  

See [LICENSE](LICENSE) for the full legal text.

---
*Built with ❤️ using C++20, OpenGL, Dear ImGui, OpenCV, and vcpkg.*
