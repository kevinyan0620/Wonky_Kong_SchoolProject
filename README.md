# Wonky Kong: C++ 2D Platformer Engine

## Project Overview
This project is a complete 2D arcade game titled "Wonky Kong". While designed as an interactive application, the underlying architecture highlights core competencies applicable to embedded systems and firmware engineering, including state machine logic, real-time event handling, and strict memory management. 

## Architecture & Tech Stack
*   **Language & Graphics:** Built entirely in C++, leveraging OpenGL/FreeGLUT for graphical rendering and keyboard interrupt event handling.
*   **Object-Oriented Design:** Engineered a comprehensive inheritance hierarchy originating from an `Actor` base class, branching into specialized `Player`, `Enemy`, `Goodie`, and `Static` object classes.
*   **Memory Management:** Implemented dynamic memory allocation handling using `std::vector` and iterators, carefully cleaning up destroyed objects per tick to prevent memory leaks.
*   **Data Parsing:** Developed a file I/O system to dynamically parse text files (`levelxx.txt`) and translate ASCII characters into instantiated game entities.

## Core Mechanics
*   **Tick-Based Game Loop:** Structured around a core controller managing `init()`, `move()`, and `cleanUp()` states, simulating a deterministic real-time operating environment.
*   **Polymorphic Behaviors:** Utilized pure virtual functions (e.g., `doSomething()`) allowing autonomous entities like `Fireball` and `Kong` to calculate trajectories and cooldowns independently.
*   **Physics & Collision Detection:** Created custom coordinate-based logic checks (`isPassable`, `isStandable`, `isClimbable`) to dictate interactions with ladders, floors, and projectiles.

## Setup & Troubleshooting

### 1. Configuring the Asset Path
The game requires access to external assets (like `.tga` image files and `.wav` sounds) to render graphics and play audio correctly. 
* Open `main.cpp` in your code editor.
* Locate the `assetDirectory` constant near the top of the file.
* Change the string to the absolute path where your `Assets` folder is located on your machine (e.g., `const string assetDirectory = "/Users/YourName/Desktop/Assets";`).

### 2. Fixing the XQuartz Display Error
If you are running this on macOS and encounter a "failed to open display" error, XQuartz is likely using a new temporary session path that no longer matches the one hardcoded in the source file.
* Ensure the XQuartz application is actively open and running on your Mac.
* Open your Terminal application and run the command `echo $DISPLAY`.
* Copy the file path output string exactly as it appears.
* Open `main.cpp` and locate the `setenv` function call.
* Replace the hardcoded string (e.g., `"/private/tmp/com.apple.launchd.../org.xquartz:0"`) with your newly copied path.

### 3. Clearing the Xcode Build Cache
If you encounter missing utility errors (like `/usr/bin/derq`) or phantom build failures, Xcode's cached build data is likely corrupted and needs to be flushed.
* Open your project in the Xcode application.
* Press `Shift + Command + K` (`⇧⌘K`) on your keyboard to clean the build folder.
* Wait a moment for Xcode to clear the Derived Data cache.
* Press `Command + B` (`⌘B`) to compile and rebuild the project from scratch.
