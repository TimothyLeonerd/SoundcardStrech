@echo off
setlocal enabledelayedexpansion

echo =========================================================
echo Soundcard_stretch Tool Installation
echo =========================================================

:: Define variables
set "MSYS2_VERSION=20251213"
set "MSYS2_EXE=msys2-x86_64-%MSYS2_VERSION%.exe"
set "MSYS2_URL=https://github.com/msys2/msys2-installer/releases/download/2025-12-13/%MSYS2_EXE%"
set "MSYS2_PATH=C:\msys64"

:: 1. MSYS2 Download
if not exist "%MSYS2_EXE%" (
    echo [1/5] Downloading MSYS2 (%MSYS2_EXE%)...
    curl -L -o "%MSYS2_EXE%" "%MSYS2_URL%"
) else (
    echo [1/5] MSYS2 installer already exists.
)

:: 2. MSYS2 Installation
if not exist "%MSYS2_PATH%" (
    echo [2/5] Installing MSYS2 to %MSYS2_PATH%...
    echo This may take a moment. Please follow the installer if it appears.
    start /wait "" "%MSYS2_EXE%" --confirm-command --accept-messages --root "%MSYS2_PATH%"
) else (
    echo [2/5] MSYS2 is already installed at %MSYS2_PATH%.
)

:: 3. Update packages and install tools
echo [3/5] Updating MSYS2 packages and installing build tools...
:: We use MSYS2 bash.exe to execute commands directly
set "MSYS_BASH=%MSYS2_PATH%\usr\bin\bash.exe"

if exist "%MSYS_BASH%" (
    :: Update package sources
    "%MSYS_BASH%" -lc "pacman -Syu --noconfirm"

    :: Re-run in case core components were updated
    echo Installing compiler, CMake, Ninja, and libraries...
    "%MSYS_BASH%" -lc "pacman -S --noconfirm mingw-w64-x86_64-gcc mingw-w64-x86_64-gdb mingw-w64-x86_64-cmake mingw-w64-x86_64-ninja mingw-w64-x86_64-pkgconf"
    "%MSYS_BASH%" -lc "pacman -S --noconfirm mingw-w64-x86_64-portaudio mingw-w64-x86_64-libsndfile mingw-w64-x86_64-rubberband"
    "%MSYS_BASH%" -lc "pacman -S --noconfirm mingw-w64-x86_64-wxwidgets3.2-msw mingw-w64-x86_64-wxwidgets3.2-msw-libs mingw-w64-x86_64-wxwidgets3.2-common"
) else (
    echo ERROR: MSYS2 Bash not found. Installation failed.
    pause
    exit /b
)

:: 4. CLion Download (JetBrains doesn't provide fixed version URLs for batch, so manual download is required)
echo [4/5] CLion preparation...
echo Please download CLion manually if you haven't already:
echo https://www.jetbrains.com/clion/download/
echo Install it and activate the non-commercial license.
pause

:: 5. Manual steps guide
echo [5/5] FINAL STEPS:
echo 1. Open CLion.
echo 2. Go to 'File | Settings | Build, Execution, Deployment | Toolchains'.
echo 3. Click '+' and select 'MinGW'.
echo 4. Set the 'Toolchain path' to: %MSYS2_PATH%\mingw64
echo 5. CMake, GCC, and GDB should be automatically detected.
echo 6. Go to 'Settings | Build, Execution, Deployment | CMake' and ensure the new toolchain is selected.
echo 7. Load project and click 'Build'.

echo =========================================================
echo Installation complete!
echo =========================================================
pause