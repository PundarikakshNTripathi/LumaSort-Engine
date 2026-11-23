<div align="center">
  <img src="assets/icons/lumasort-logo.svg" alt="LumaSort Engine Logo" width="128">
  <h1>LumaSort-Engine</h1>
  <p>Turn any source media (image, webcam feed, drawing) into any target image through global luminance-based optimal transport.</p>

  <p>
    <a href="https://github.com/PundarikakshNTripathi/LumaSort-Engine/actions"><img src="https://img.shields.io/badge/Build-Passing-2ea44f?style=for-the-badge&logo=githubactions&logoColor=white" alt="Build Status"></a>
    <a href="LICENSE"><img src="https://img.shields.io/badge/License-Apache%202.0-D22128?style=for-the-badge&logo=apache&logoColor=white" alt="Apache 2.0 License"></a>
    <a href="https://github.com/PundarikakshNTripathi/LumaSort-Engine/releases"><img src="https://img.shields.io/badge/Platform-Windows%20%7C%20Linux-blue?style=for-the-badge&logo=linux&logoColor=white" alt="Platform Support"></a>
  </p>

  <p>
    <a href="https://en.cppreference.com/w/cpp/20"><img src="https://img.shields.io/badge/C%2B%2B-20-00599C?style=for-the-badge&logo=c%2B%2B&logoColor=white" alt="C++20"></a>
    <a href="https://www.opengl.org/"><img src="https://img.shields.io/badge/OpenGL-3.3%20Core-5586A4?style=for-the-badge&logo=opengl&logoColor=white" alt="OpenGL 3.3 Core"></a>
    <a href="https://github.com/ocornut/imgui"><img src="https://img.shields.io/badge/Dear_ImGui-Docking-7C3AED?style=for-the-badge&logo=c%2B%2B&logoColor=white" alt="Dear ImGui Docking"></a>
    <a href="https://opencv.org/"><img src="https://img.shields.io/badge/OpenCV-4.x-5C3EE8?style=for-the-badge&logo=opencv&logoColor=white" alt="OpenCV 4.x"></a>
    <a href="https://github.com/btzy/nativefiledialog-extended"><img src="https://img.shields.io/badge/NFD-Extended-4A90A4?style=for-the-badge" alt="NFD Extended"></a>
    <a href="https://cmake.org/"><img src="https://img.shields.io/badge/CMake-3.20%2B-064F8C?style=for-the-badge&logo=cmake&logoColor=white" alt="CMake 3.20+"></a>
    <a href="https://vcpkg.io/"><img src="https://img.shields.io/badge/vcpkg-Manifest%20Mode-5C2D91?style=for-the-badge&logo=vcpkg&logoColor=white" alt="vcpkg Manifest Mode"></a>
  </p>
</div>

> [!Note]
> **Copyright (c) 2025-2026 Pundarikaksh Narayan Tripathi**  
> This project is licensed under the **Apache License 2.0**.  
> See the [LICENSE](LICENSE) file for details.

<p align="center">
  <img src="assets/images/lumasort_bocchi_kita.gif" alt="LumaSort Bocchi to Kita Pixel Sorting Demo" width="640" />
</p>

---

## Introduction

**LumaSort Engine** is a high-performance C++20 real-time visualizer that rearranges pixels from live video feeds, static images, or real-time drawings into a target image based on global luminance intensity. By combining a "Pixel Sorting" algorithm with Fluid Dynamics (Flow Fields), it creates a mesmerizing, fluid-like transition of pixels finding their new "home" based on brightness.

The engine features a complete GUI control panel with native file dialogs, real-time physics parameter tuning, and an interactive canvas with VIBGYOR color palette for drawing. Users can load source and target images, adjust particle behavior, and watch the transformation unfold with a single button click.

### Inspiration & Backstory

This project was inspired by **Spu7Nix's Obama turning app** in his video: [I made a program that turns anything into Obama](https://youtu.be/MeFi68a2pP8?si=WczfFm_GjwLiYxr6), which I stumbled upon last year. Back then, I couldn't find the source code repository anywhere online, but thought the concept was super cool and wanted to try building it myself. So I ended up reverse-engineering it on my own from scratch in modern C++.

While inspired by the original video, LumaSort is a bit different: it lets you turn any image into any other image, hooks into live webcam feeds, and supports interactive real-time canvas drawing. I originally planned to record a video about the project and how it works, but got lazy and ended up never getting around to making it; so here is the engine and code for anyone to explore!

---

## Table of Contents

- [Introduction](#introduction)
- [Features](#features)
- [How It Works](#how-it-works)
- [Architecture](#architecture)
- [Tech Stack](#tech-stack)
- [Directory Structure](#directory-structure)
- [Download & Run](#download--run)
- [Build from Source](#build-from-source)
- [Usage Guide](#usage-guide)
- [Troubleshooting](#troubleshooting)
- [Known Limitations & Future Work](#known-limitations--future-work)
- [License](#license)

---

## Features

- **Multi-Input Support**: Live Webcam, Static Images, or Interactive Drawing Canvas
- **Native File Dialogs**: OS-native file pickers for loading source and target images
- **Interactive Canvas**: Draw with VIBGYOR color palette, adjustable brush sizes, and eraser tool
- **Transform Control**: Start/Stop transformation with dedicated button - preview content before animating
- **Adaptive Resolution**: Automatically adjusts particle count based on source image resolution
- **Responsive Viewport**: Dynamic scaling on window resize with aspect ratio preservation
- **Real-time Visualization**: High-performance sorting at 60+ FPS
- **Physics Parameter Tuning**: Adjust particle speed, flow strength, and noise scale in real-time
- **Fluid Dynamics**: Pixels move organically using Flow Fields (Perlin/Simplex Noise)
- **Custom Branding**: Application icon in taskbar and Alt-Tab switcher

---

## How It Works

1.  **Input Selection**: Choose between Webcam, Image, or Canvas mode via the GUI dropdown
2.  **Source Loading**: Load images using native file dialogs or draw on the canvas
3.  **Target Loading**: Select a target image that defines the final shape/pattern
4.  **Preview**: View your source content as a stable image before transformation
5.  **Transform**: Click "Start Transform" to begin the particle animation
    - **Luminance Sorting**: Each source pixel is matched to a target pixel of similar brightness
    - **Steering Forces**: Particles feel a pull towards their sorted destination
    - **Flow Field**: Background vector field adds organic turbulence to the motion
6.  **Convergence**: Particles gradually settle into the target shape, recreating the image

---

## Architecture

The engine follows a strict separation of concerns with a complete GUI layer for user interaction.

![Architecture Diagram](assets/images/architecture.svg)

---

## Tech Stack

| Component | Technology | Reasoning |
| :--- | :--- | :--- |
| **Language** | C++20 | High-performance memory management and modern language features. |
| **Build System** | CMake | Industry standard for cross-platform C++ build configuration. |
| **Package Manager** | vcpkg | Seamless integration of libraries in "Manifest Mode" for reproducible builds. |
| **Computer Vision** | OpenCV 4 | Robust library for webcam feeds and efficient image matrix manipulation. |
| **Rendering** | OpenGL 3.3+ | Hardware acceleration for rendering millions of particles at 60 FPS. |
| **Windowing** | GLFW + GLAD | Lightweight, standard way to create contexts and handle input. |
| **UI** | Dear ImGui | Immediate Mode GUI for real-time parameter tuning and controls. |
| **File Dialogs** | NFD Extended | Cross-platform native file dialogs for OS-integrated file selection. |
| **Math** | GLM | Standard mathematics library for graphics (vectors, matrices, noise). |

---

## Directory Structure

```text
lumasort-engine/
├── vcpkg.json              # Dependency Manifest (OpenCV, ImGui, NFD, etc.)
├── CMakeLists.txt          # Build Configuration
├── LumaSort.desktop        # Linux Desktop Entry (for app launchers)
├── src/
│   ├── main.cpp            # Entry Point
│   ├── app.h/cpp           # Application Loop, State, & Transform Logic
│   ├── core/
│   │   ├── sorter.h/cpp    # Luminance-based Pixel Sorting Algorithm
│   │   ├── particle.h      # Particle Entity Structure
│   │   └── flow_field.h/cpp# Fluid Math (Perlin/Simplex Noise)
│   ├── graphics/
│   │   ├── renderer.h/cpp  # OpenGL Particle Rendering
│   │   ├── texture.h/cpp   # Texture Management & OpenCV Upload
│   │   └── canvas.h/cpp    # FBO Drawing Surface with Color Support
│   └── ui/
│       └── gui_layer.h/cpp # ImGui Control Panel & Native File Dialogs
├── assets/
│   ├── icons/              # Application Icons
│   ├── shaders/            # GLSL Vertex & Fragment Shaders
│   └── images/             # Sample Images
└── build/                  # (Generated) Build artifacts
```

---

## Download & Run

Pre-built releases are available for **Windows** and **Linux**. No compilation required!

### Download

Go to the [**Releases Page**](https://github.com/PundarikakshNTripathi/LumaSort-Engine/releases) and download the latest version for your OS.

### Windows

1. Download `LumaSort-windows-x64.zip`
2. Extract the ZIP file
3. Double-click `LumaSort.exe` to run

### Linux

1. Download `LumaSort-linux-x86_64.tar.gz`
2. Extract and run:
```bash
tar -xzvf LumaSort-linux-x86_64.tar.gz
cd LumaSort-linux
./run.sh
```

> [!Note]
> The `run.sh` script sets up the library paths automatically. Always use it to launch the application.

### macOS

macOS pre-built binaries are not available in this release due to CI infrastructure limitations. **macOS users can build from source** - see [Build from Source](#build-from-source) below.

> [!Note]
> **Future Work:** Native macOS builds (Intel and Apple Silicon) will be added when the vcpkg glad port is updated for CMake 4.x compatibility.

---

## Build from Source

If you want to modify the code or build for a different platform, follow these instructions.

### 1. Requirements

**Essential Build Tools:**
- C++ Compiler (GCC 11+ / Clang 14+)
- CMake (3.20+)
- Ninja or Make

**System Libraries (Linux):**
- **X11 / OpenGL**: `libx11-dev`, `libglu1-mesa-dev`
- **GTK3** (for native file dialogs): `libgtk-3-dev`
- **Build Helpers**: `bison`, `flex`, `gperf`, `pkg-config`
- **Python Build Environment**: `python3-venv`

### 2. Installation (One-line)
```bash
sudo apt update && sudo apt install -y build-essential pkg-config cmake ninja-build autoconf autoconf-archive automake libtool bison flex gperf libx11-dev libxext-dev libxrandr-dev libxinerama-dev libxcursor-dev libxi-dev libglu1-mesa-dev libgl1-mesa-dev libgtk-3-dev python3-venv python3-jinja2
```

### 3. Build & Run

1.  **Clone the Repository**
    ```bash
    git clone https://github.com/PundarikakshNTripathi/LumaSort-Engine.git
    cd LumaSort-Engine
    ```

2.  **Configure (CMake + vcpkg)**
    ```bash
    cmake -B build -S . -DCMAKE_TOOLCHAIN_FILE=/path/to/vcpkg/scripts/buildsystems/vcpkg.cmake
    ```
    *(Note: First build downloads and compiles all dependencies - may take 5-10 minutes)*

3.  **Compile**
    ```bash
    cmake --build build
    ```

4.  **Run**
    ```bash
    ./build/LumaSort
    ```

---

## Usage Guide

### Input Modes

1. **Webcam**: Live camera feed (adapts to webcam resolution, capped at 600px)
2. **Image**: Load any image file - resolution adapts automatically (up to 800px)
3. **Canvas**: Draw with VIBGYOR colors using pen/eraser tools

### Workflow

1. Select input mode from the dropdown
2. Load a **Target Image** using the native file dialog
3. For Image mode: Load a **Source Image**
4. For Canvas mode: Draw your design using the color palette
5. Click **Start Transform** to begin the animation
6. Adjust **Physics Parameters** (Speed, Flow Strength, Noise Scale) in real-time
7. Click **Stop Transform** to reset and try again

### Physics Parameters

| Parameter | Description | Range |
| :--- | :--- | :--- |
| Particle Speed | How fast particles move toward targets | 0.001 - 0.1 |
| Flow Strength | Intensity of turbulent flow field | 0.0001 - 0.01 |
| Noise Scale | Size of flow field patterns | 1.0 - 20.0 |

---

## Troubleshooting

### `vcpkg install failed` with `python3 -m venv failed`
**Fix:** Install the missing python module:
```bash
sudo apt install python3-venv
```

### `Could not find bison` or `GPERF not found`
**Fix:** Install build tool suite:
```bash
sudo apt install bison flex gperf
```

### Native file dialogs not working
**Cause:** GTK3 development libraries are required for native file dialogs on Linux.
**Fix:**
```bash
sudo apt install libgtk-3-dev
```

### `CMake Error: CMAKE_MAKE_PROGRAM is not set`
**Fix:**
```bash
sudo apt install build-essential ninja-build
```

### Low FPS with high-resolution images
**Cause:** Very large images (5000x3000+) generate hundreds of thousands of particles.
**Note:** This is expected behavior - the engine caps at 800px for images to maintain performance.

### Vertical black lines on Intel HD graphics
**Cause:** Fixed point size rendering causes gaps between particles on certain GPU/resolution combinations.
**Status:** Fixed in v1.0.1. If you see this issue, ensure you're running the latest version.

---

## Known Limitations & Future Work

### macOS Builds Not Available

**Why:** GitHub Actions free tier macOS runners (macos-14/15) come with CMake 4.x, which is incompatible with the `glad` vcpkg port that uses deprecated CMake 3.0 syntax. Paid Intel runners would work but aren't available on the free tier.

**Workaround:** macOS users can [build from source](#build-from-source) - the project builds successfully on macOS with Homebrew dependencies.

### Future Work

| Priority | Feature | Status |
|----------|---------|--------|
| High | Native macOS builds (Intel + Apple Silicon) | Waiting for vcpkg glad fix |
| Medium | GPU-accelerated particle physics (compute shaders) | Planned |
| Medium | Video export functionality | Planned |
| Low | Custom flow field patterns | Planned |

---

## License

This project is licensed under the **Apache License 2.0**.

**What this means:**

- You are free to use, modify, distribute, and sell this software for personal or commercial purposes.
- You must include the original copyright notice and Apache 2.0 license in any distribution.
- Derivative works may be distributed under different terms, provided attribution is maintained.

See [LICENSE](LICENSE) for the full legal text.
