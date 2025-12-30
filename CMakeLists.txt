cmake_minimum_required(VERSION 3.20)

project(Soundcard_wav
    VERSION 0.1.0
    LANGUAGES CXX
)

# ---- Global C++ options ----

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Helpful on MSVC to get better warnings, etc.
if (MSVC)
    add_compile_options(/permissive- /W4)
endif()

# ---- Dependencies ----
# Assumptions: using vcpkg and configure CMake with its toolchain, e.g.:
#
#   cmake -S . -B build -DCMAKE_TOOLCHAIN_FILE="C:/vcpkg/scripts/buildsystems/vcpkg.cmake"
#
# Installed via:
#   vcpkg install wxwidgets portaudio rubberband

# 1) wxWidgets (vcpkg provides a CMake config with wx::core, wx::base targets)
find_package(wxWidgets CONFIG REQUIRED)

# 2) PortAudio (we don’t assume a specific imported target name, just find headers + lib)
find_path(PORTAUDIO_INCLUDE_DIR
    NAMES portaudio.h
    PATH_SUFFIXES include
)

find_library(PORTAUDIO_LIBRARY
    NAMES portaudio portaudio_static
)

if (NOT PORTAUDIO_INCLUDE_DIR OR NOT PORTAUDIO_LIBRARY)
    message(FATAL_ERROR "PortAudio not found. Make sure it is installed (e.g. via vcpkg) and visible to CMake.")
endif()

# 3) Rubber Band library
find_path(RUBBERBAND_INCLUDE_DIR
    NAMES rubberband/RubberBandStretcher.h
    PATH_SUFFIXES include
)

find_library(RUBBERBAND_LIBRARY
    NAMES rubberband
)

if (NOT RUBBERBAND_INCLUDE_DIR OR NOT RUBBERBAND_LIBRARY)
    message(FATAL_ERROR "Rubber Band library not found. Make sure it is installed (e.g. via vcpkg) and visible to CMake.")
endif()

# ---- Sources ----

set(SC_SOURCES
    wx_test.cpp
    main_window.cpp
    my_events.cpp
    audio_recorder.cpp
    paex_record.cpp
    playback.cpp
    play_button.cpp
    record_button.cpp
    sawtooth_playback.cpp
    state.cpp
    utils.cpp
    wave_panel.cpp
)

set(SC_HEADERS
    main_window.h
    my_events.h
    audio_recorder.h
    playback.h
    play_button.h
    record_button.h
    state.h
    utils.h
    wave_panel.h
)

# ---- Executable ----
# WIN32: build as a GUI app on Windows (no default console window).
# You explicitly create a console in SetupConsole(), so this is fine.
add_executable(Soundcard_wav WIN32
    ${SC_SOURCES}
    ${SC_HEADERS}
)

# Where headers live
target_include_directories(Soundcard_wav
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${PORTAUDIO_INCLUDE_DIR}
        ${RUBBERBAND_INCLUDE_DIR}
)

# Link everything
target_link_libraries(Soundcard_wav
    PRIVATE
        wx::core
        wx::base
        ${PORTAUDIO_LIBRARY}
        ${RUBBERBAND_LIBRARY}
)

# Optional but nice: put built binary in build/bin
set_target_properties(Soundcard_wav PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)
