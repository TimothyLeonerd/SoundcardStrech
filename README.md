# SoundcardStrech

A small Windows app for recording, visualizing, and playing back audio using:

- **wxWidgets** – GUI  
- **PortAudio** – audio I/O  
- **RubberBand** – time-stretching / pitch-shifting  

The project is configured to use:

- **CMake** for the build system and **wxWidgets** checkout
- **MSYS2 MINGW64** to automatically fetch PortAudio, RubberBand, and all dependencies for the build

## Overview

SoundcardStrech allows users to capture audio from their soundcard, visualize the waveform,
and play it back using high-quality DSP algorithms. It is designed to be lightweight
and easy to build using modern CMake and standard package managers.

### Key Features
- **Audio Recording:** Capture high-quality audio via PortAudio.
- **Waveform Visualization:** Real-time rendering of audio data.
- **Time-Stretching:** Change playback speed without affecting pitch using the Rubber Band Library.
- **Cross-Platform:** Supports Windows (via MSYS2/MinGW), Linux, and macOS.

## Technologies

The project relies on the following libraries:
- **wxWidgets** – For the graphical user interface.
- **PortAudio** – For cross-platform audio input and output.
- **RubberBand** – For high-quality time-stretching and pitch-shifting.
- **libsndfile** – For handling audio file formats.

## Getting Started

### Prerequisites
To build this project, you need:
- A C++17 compatible compiler (GCC, Clang).
- **CMake** (version 3.24 or higher).
- **Ninja** (recommended build generator).

### Installation & Setup
The setup process has been simplified. We no longer use Visual Studio or vcpkg.

For detailed, step-by-step instructions for your operating system, please refer to the:

**[INSTALL.md - Full Installation Guide](doc/INSTALL.md)**

#### Quick Start (Windows)
1. Clone the repository.
2. Run `tool_install.bat` as Administrator to install all dependencies via MSYS2.
3. Open the project in **CLion** and configure the MinGW toolchain as described in the guide.

## Building from Command Line

Once the dependencies from `INSTALL.md` are installed, you can build the project manually:

```bash
# Create build directory
cmake -S . -B build -G Ninja
# Build the project
cmake --build build
# Run the application
./build/Soundcard_wav
```

## Project Structure
- `Soundcard_wav/`: Contains the C++ source and header files.
- `icons/`: UI resources and assets.
- `tool_install.bat`: Automated setup script for Windows users.
- `CMakeLists.txt`: The main build configuration.

---

## License
This project is for educational/non-commercial use. Please check the individual library licenses (wxWidgets, PortAudio, Rubber Band) for further details.