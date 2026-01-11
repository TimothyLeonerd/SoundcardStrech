cmake_minimum_required(VERSION 3.24)
project(Soundcard_wav LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

include(ExternalProject)

# ------------------------------------------------------------
# Sources (from your repo)
#   NOTE: paex_record.cpp and sawtooth_playback.cpp also have main(),
#   so we do NOT include them in this GUI build.
# ------------------------------------------------------------
set(SC_SOURCES
        audio_recorder.cpp
        main_window.cpp
        my_events.cpp
        playback.cpp
        play_button.cpp
        record_button.cpp
        state.cpp
        utils.cpp
        wave_panel.cpp
        wx_test.cpp
)

set(SC_HEADERS
        audio_recorder.h
        main_window.h
        my_events.h
        playback.h
        play_button.h
        record_button.h
        state.h
        utils.h
        wave_panel.h
)

# ------------------------------------------------------------
# wxWidgets (build from source, install into build dir)
# ------------------------------------------------------------
set(WX_INSTALL_DIR "${CMAKE_BINARY_DIR}/wx-install")
set(WX_LIB_DIR     "${WX_INSTALL_DIR}/lib/gcc_x64_lib")
set(WX_SETUP_DIR   "${WX_LIB_DIR}/mswu")
set(WX_INC_DIR     "${WX_INSTALL_DIR}/include")

ExternalProject_Add(wxWidgets
        GIT_REPOSITORY https://github.com/wxWidgets/wxWidgets.git
        GIT_TAG v3.2.9
        PREFIX "${CMAKE_BINARY_DIR}/wxWidgets"
        SOURCE_DIR "${CMAKE_BINARY_DIR}/wxWidgets/src/wxWidgets"
        BINARY_DIR "${CMAKE_BINARY_DIR}/wxWidgets/src/wxWidgets-build"
        CMAKE_ARGS
        -DCMAKE_INSTALL_PREFIX=${WX_INSTALL_DIR}
        -DCMAKE_BUILD_TYPE=Release
        -DwxBUILD_SHARED=OFF
        -DCMAKE_POLICY_VERSION_MINIMUM=3.5
        -G Ninja
        BUILD_COMMAND ${CMAKE_COMMAND} --build . --target install
        UPDATE_DISCONNECTED 1
        # Critical for Ninja: tell it these files will be produced by wxWidgets target
        BUILD_BYPRODUCTS
        "${WX_LIB_DIR}/wxmsw32u_core.a"
        "${WX_LIB_DIR}/wxbase32u.a"
        "${WX_LIB_DIR}/wxmsw32u_adv.a"
        "${WX_LIB_DIR}/wxpng.a"
        "${WX_LIB_DIR}/wxzlib.a"
)

set(WINDOWS_LIBS
        comctl32
        shlwapi
        uxtheme
        oleacc
        version
)

add_library(wx_core_lib INTERFACE)
target_link_libraries(wx_core_lib INTERFACE
        "${WX_LIB_DIR}/wxmsw32u_core.a"
        "${WX_LIB_DIR}/wxbase32u.a"
        "${WX_LIB_DIR}/wxmsw32u_adv.a"
        "${WX_LIB_DIR}/wxpng.a"
        "${WX_LIB_DIR}/wxzlib.a"
        ${WINDOWS_LIBS}
)

# ------------------------------------------------------------
# rubberband + portaudio (via pkg-config from MSYS2)
# ------------------------------------------------------------
find_package(PkgConfig REQUIRED)
pkg_check_modules(RUBBERBAND REQUIRED IMPORTED_TARGET rubberband)
pkg_check_modules(PORTAUDIO  REQUIRED IMPORTED_TARGET portaudio-2.0)

# ------------------------------------------------------------
# Executable
# ------------------------------------------------------------
add_executable(Soundcard_wav WIN32
        ${SC_SOURCES}
        ${SC_HEADERS}
)

target_include_directories(Soundcard_wav PRIVATE
        "${WX_INC_DIR}"
        "${WX_SETUP_DIR}"
)

target_link_libraries(Soundcard_wav PRIVATE
        wx_core_lib
        PkgConfig::RUBBERBAND
        PkgConfig::PORTAUDIO
)

target_compile_options(Soundcard_wav PRIVATE
        -Wall -Wextra -Wpedantic
)

# Make sure wxWidgets builds before linking the exe
add_dependencies(Soundcard_wav wxWidgets)

# Run/debug from repo root (so relative paths work)
set_target_properties(Soundcard_wav PROPERTIES
        VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
)

# Copy icons next to exe
add_custom_command(TARGET Soundcard_wav POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${CMAKE_SOURCE_DIR}/icons"
        "$<TARGET_FILE_DIR:Soundcard_wav>/icons"
)

target_compile_definitions(Soundcard_wav PRIVATE
        ICONS_DIR="$<TARGET_FILE_DIR:Soundcard_wav>/icons"
)
