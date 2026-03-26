# Smart City Map

## 🎮 Demo Video

Watch the Smart City map in action via the latest [GitHub Release](https://github.com/USERNAME/REPO/releases/download/v1.0.0/map_demo.mp4).

> The video showcases the layout of the city, building placement, traffic lights, and interactive elements.

This repository contains the **3D map of the Smart City project** that I designed as part of our team project. The map is implemented in C++ using **Raylib**, featuring buildings, roads,  and various facilities.

## Project Overview

The Smart City project is a collaborative simulation aimed at modeling urban environments with:

- Roads, intersections, roundabouts, and parking areas
- Residential, commercial, and public buildings
- Emergency, medical, and educational zones
-  systems for intersections

This repository focuses exclusively on the **map design and building placement**.

## My Role

- Designed and implemented the city map with a **10x10 tile system**.
- Placed buildings and facilities on grass tiles, including restaurants, hospitals, schools, and police/fire stations.
- Defined building dimensions, orientation, and textures for both day and night scenarios.
- Collaborated with teammates to integrate this map into the full Smart City simulator.

## Map Details

- **TileMap**: 10x10 grid representing roads, grass, roundabouts, and parking
- **FacilityMap**: Buildings and facilities mapped to tiles
- **Building Types**: Restaurant, Coffee Shops, Hospitals, Schools, Police/Fire Stations, Shops, Bookstore, Boutique, etc.

## Technologies

- **C++**
- **Raylib** (3D rendering and game engine utilities)
- **Raymath** (vector/matrix operations)
- Textures and models for realistic building and road representation

## How to Build and Run

This project uses **CMake** to configure and build the 3D city map. Follow these steps:



```powershell
1. **Clean the old build directory** (recommended for linker fixes):
Remove-Item -Path .\build -Recurse -Force


2. **Re-configure the project:**
cmake -B build

3.**Build the project:**
cmake --build build --config Debug

4.**Run the program:**
cd build/Debug
.\CitySmart.exe

Ensure that Raylib and any required textures/models are available in the project directory before running.

Map Details
Tile Map: Roads, intersections, grass, playgrounds, and parking spaces.
Facility Map: Buildings and facilities placed on the grid with orientation.
3D Rendering: Each building is drawn with front, back, and side textures, including window glow effects at night.
Traffic Lights: Positioned at intersections with red, orange, and green states.
```
The full Smart City project is available in the [organization repository](https://github.com/RaylibSmartCity-Devs/CitySmart-).
