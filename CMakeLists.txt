cmake_minimum_required(VERSION 3.24)

include(ExternalProject)

ExternalProject_Add(wxWidgets
        GIT_REPOSITORY https://github.com/wxWidgets/wxWidgets.git
        GIT_TAG v3.2.9
        PREFIX ${CMAKE_BINARY_DIR}/wxWidgets
        SOURCE_DIR ${CMAKE_BINARY_DIR}/wxWidgets/src/wxWidgets
        BINARY_DIR ${CMAKE_BINARY_DIR}/wxWidgets/src/wxWidgets-build
        CMAKE_ARGS
        -DCMAKE_INSTALL_PREFIX=${CMAKE_BINARY_DIR}/wx-install
        -DCMAKE_BUILD_TYPE=Release
        -DwxBUILD_SHARED=OFF
        -DCMAKE_POLICY_VERSION_MINIMUM=3.5
        -G Ninja
        BUILD_COMMAND ${CMAKE_COMMAND} --build . --target install
        UPDATE_DISCONNECTED 1
)

set(windows_LIBRARIES
        comctl32
        shlwapi
        uxtheme
        oleacc
        version
)

add_library(wx_core_lib INTERFACE)

target_link_libraries(wx_core_lib INTERFACE
        "${CMAKE_BINARY_DIR}/wx-install/lib/gcc_x64_lib/wxmsw32u_core.a"
        "${CMAKE_BINARY_DIR}/wx-install/lib/gcc_x64_lib/wxbase32u.a"
        "${CMAKE_BINARY_DIR}/wx-install/lib/gcc_x64_lib/wxmsw32u_adv.a"
        "${CMAKE_BINARY_DIR}/wx-install/lib/gcc_x64_lib/wxpng.a"
        "${CMAKE_BINARY_DIR}/wx-install/lib/gcc_x64_lib/wxzlib.a"
        ${windows_LIBRARIES}
)

add_dependencies(wx_core_lib wxWidgets)

set(wxWidgets_INCLUDE_DIR "${CMAKE_BINARY_DIR}/wx-install/include")
set(wxWidgets_SETUP_DIR "${CMAKE_BINARY_DIR}/wx-install/lib/gcc_x64_lib/mswu")

find_package(PkgConfig REQUIRED)

pkg_check_modules(RUBBERBAND REQUIRED rubberband)
pkg_check_modules(PORTAUDIO REQUIRED portaudio-2.0)

project(Soundcard_wav LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_definitions(-D__WXMSW__ -DwxUSE_UNICODE)

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

add_executable(Soundcard_wav WIN32
    ${SC_SOURCES}
    ${SC_HEADERS}
)

target_include_directories(Soundcard_wav PRIVATE ${wxWidgets_INCLUDE_DIR} ${wxWidgets_SETUP_DIR} ${RUBBERBAND_INCLUDE_DIRS} ${PORTAUDIO_INCLUDE_DIRS})
target_link_libraries(Soundcard_wav PRIVATE wx_core_lib ${RUBBERBAND_LIBRARIES} ${PORTAUDIO_LIBRARIES})
target_compile_options(Soundcard_wav PRIVATE -Wall -Wextra -Wpedantic ${RUBBERBAND_CFLAGS_OTHER})

set_target_properties(Soundcard_wav PROPERTIES
        VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
)

add_custom_command(TARGET Soundcard_wav POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory "${CMAKE_SOURCE_DIR}/icons" "$<TARGET_FILE_DIR:Soundcard_wav>/icons"
)

target_compile_definitions(Soundcard_wav PRIVATE
    ICONS_DIR="$<TARGET_FILE_DIR:Soundcard_wav>/icons"
)
