# petproject
# Aim Trainer (C++ / SDL2)

An interactive aim training game built in C++ using the SDL2 library, designed to improve reaction time, micro-flicks, and shooting accuracy.

## 🚀 Key Features
- **Fullscreen Support:** Dynamically scales to fit your desktop resolution using `SDL_WINDOW_FULLSCREEN_DESKTOP`.
- **Game State Management:** Includes a clean main menu and an active gameplay loop.
- **Dynamic Targets:** Targets spawn randomly with varying radii across the screen boundaries.
- **Real-time Statistics:** Tracks your Score, remaining Time (30-second rounds), and real-time Accuracy percentage (`Accuracy = (Score / Total Clicks) * 100`).
- **Interactive Controls:** 
  - `ENTER` to start the game from the menu.
  - `R` to restart quickly after a game-over session.
  - `ESC` for an instant emergency exit from the game.

## 🛠 Tech Stack
- **Language:** C++
- **Graphics & Events:** SDL2
- **Text Rendering:** SDL2_ttf

## 📦 How to Build & Run (macOS)
1. Make sure you have the required SDL2 libraries and CMake installed (via Homebrew):
   ```bash
   brew install sdl2 sdl2_ttf cmake
   mkdir build
   cd build
   cmake ..
   make
   ./AimTrainer