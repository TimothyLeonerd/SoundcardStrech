@echo off
setlocal

echo =========================================================
echo Soundcard_stretch Tool Installation
echo =========================================================

rem -----------------------------
rem Define variables
rem -----------------------------
set "MSYS2_VERSION=20251213"
set "MSYS2_EXE=msys2-x86_64-%MSYS2_VERSION%.exe"
set "MSYS2_URL=https://github.com/msys2/msys2-installer/releases/download/2025-12-13/%MSYS2_EXE%"
set "MSYS2_PATH=C:\msys64"
set "MSYS_BASH=%MSYS2_PATH%\usr\bin\bash.exe"

rem -----------------------------
rem 1. MSYS2 Download
rem -----------------------------
if exist "%MSYS2_EXE%" goto :MSYS2_DOWNLOADED

echo [1/5] Downloading MSYS2 (%MSYS2_EXE%)...
curl.exe -L -o "%MSYS2_EXE%" "%MSYS2_URL%"
if errorlevel 1 goto :ERR_DOWNLOAD

:MSYS2_DOWNLOADED
if exist "%MSYS2_EXE%" (
  echo [1/5] MSYS2 installer ready: %MSYS2_EXE%
) else (
  echo [1/5] MSYS2 installer missing after download.
  goto :ERR_DOWNLOAD
)

rem -----------------------------
rem 2. MSYS2 Installation
rem -----------------------------
if exist "%MSYS2_PATH%" goto :MSYS2_INSTALLED

echo [2/5] Installing MSYS2 to %MSYS2_PATH%...
echo This may take a moment. Please follow the installer if it appears.
start /wait "" "%MSYS2_EXE%" --confirm-command --accept-messages --root "%MSYS2_PATH%"

rem Verify
if not exist "%MSYS2_PATH%" goto :ERR_INSTALL

:MSYS2_INSTALLED
echo [2/5] MSYS2 is installed at %MSYS2_PATH%.

rem -----------------------------
rem 3. Update packages and install tools
rem -----------------------------
echo [3/5] Updating MSYS2 packages and installing build tools...

if not exist "%MSYS_BASH%" goto :ERR_NO_BASH

rem Update system packages (may update pacman/core)
"%MSYS_BASH%" -lc "pacman -Syu --noconfirm"
if errorlevel 1 goto :ERR_PACMAN

echo Installing compiler, CMake, Ninja, and libraries...
"%MSYS_BASH%" -lc "pacman -S --noconfirm mingw-w64-x86_64-gcc mingw-w64-x86_64-gdb mingw-w64-x86_64-cmake mingw-w64-x86_64-ninja mingw-w64-x86_64-pkgconf"
if errorlevel 1 goto :ERR_PACMAN

"%MSYS_BASH%" -lc "pacman -S --noconfirm mingw-w64-x86_64-portaudio mingw-w64-x86_64-libsndfile mingw-w64-x86_64-rubberband"
if errorlevel 1 goto :ERR_PACMAN

"%MSYS_BASH%" -lc "pacman -S --noconfirm mingw-w64-x86_64-wxwidgets3.2-msw mingw-w64-x86_64-wxwidgets3.2-msw-libs mingw-w64-x86_64-wxwidgets3.2-common"
if errorlevel 1 goto :ERR_PACMAN

rem -----------------------------
rem 4. CLion Download (manual)
rem -----------------------------
echo [4/5] CLion preparation...
echo Please download CLion manually if you haven't already:
echo https://www.jetbrains.com/clion/download/
echo Install it and activate the non-commercial license.
pause

rem -----------------------------
rem 5. Manual steps guide
rem -----------------------------
echo [5/5] FINAL STEPS:
echo 1. Open CLion.
echo 2. Go to 'File ^| Settings ^| Build, Execution, Deployment ^| Toolchains'.
echo 3. Click '+' and select 'MinGW'.
echo 4. Set 'Toolset' to: %MSYS2_PATH%\mingw64
echo 5. CMake, GCC, and GDB should be automatically detected.
echo 6. Go to 'Settings ^| Build, Execution, Deployment ^| CMake' and ensure the new toolchain is selected.
echo 7. Load project and click 'Build'.

echo =========================================================
echo Installation complete!
echo =========================================================
pause
exit /b 0

rem -----------------------------
rem Error handlers
rem -----------------------------
:ERR_DOWNLOAD
echo ERROR: MSYS2 download failed.
echo URL: %MSYS2_URL%
pause
exit /b 1

:ERR_INSTALL
echo ERROR: MSYS2 installation failed or did not create: %MSYS2_PATH%
pause
exit /b 1

:ERR_NO_BASH
echo ERROR: MSYS2 Bash not found at:
echo   %MSYS_BASH%
echo Installation may have failed.
pause
exit /b 1

:ERR_PACMAN
echo ERROR: pacman command failed.
echo You may need to open the MSYS2 terminal once and run updates manually, then re-run this script.
pause
exit /b 1
