# SoundcardStrech

A small Windows app for recording, visualizing, and playing back audio using:

- **wxWidgets** – GUI  
- **PortAudio** – audio I/O  
- **Rubber Band Library** – time-stretching / pitch-shifting  

The project is configured to use:

- **CMake** for the build system  
- **vcpkg (manifest mode)** to automatically fetch wxWidgets, PortAudio, Rubber Band, and their dependencies  

Goal:

> On a reasonably recent Windows machine:  
> **Clone → follow the steps → get a working `.exe`.**

---

## 1. Prerequisites (one-time setup)

These are things you may or may not already have. If you do, you can skip the corresponding steps.

### 1.1 Git

If you don’t already have Git:

1. Download **Git for Windows** from the official site.  
2. Install with default options.  
3. Open **Command Prompt** or **PowerShell** and check:

   ```cmd
   git --version
   ```

If it prints a version, Git is installed correctly.

---

### 1.2 Visual Studio (C++ toolchain + CMake)

You need **Visual Studio Community 2022 or newer** (e.g. 2026) with C++ support and CMake tools.

If Visual Studio is **not installed** yet:

1. Download **Visual Studio Community** from Microsoft.  
2. During installation, select the workload:

   - ✅ **“Desktop development with C++”**

3. Then go to the **Individual components** tab and ensure:

   - ✅ **“C++ CMake tools for Windows”**  
   - (Optional but recommended) a recent **Windows 10/11 SDK**

If Visual Studio **is installed** already:

1. Open **Visual Studio Installer**.  
2. Click **Modify** on your VS installation.  
3. Ensure that:  
   - **“Desktop development with C++”** is checked.  
   - **“C++ CMake tools for Windows”** is checked.  
4. Apply changes and close the installer.  
5. Close and reopen any terminals after modifying VS.

---

## 2. Clone the repository

Open **Command Prompt** or **PowerShell** and choose where you want the repo (e.g. `C:\Users\<YourUser>\source\repos`):

```cmd
cd C:\Users\<YourUser>\source\repos

git clone https://github.com/TimothyLeonerd/SoundcardStrech.git
cd SoundcardStrech
```

The repo includes vcpkg as a submodule. Initialize it:

```cmd
git submodule update --init --recursive
```

After this, you should see at least:

- `CMakeLists.txt`  
- `vcpkg/`  
- `vcpkg.json`  
- `Soundcard_wav/` (sources live here)  
- various `.cpp` / `.h` files for the app  

---

## 3. Recommended build path (Windows, x64, with vcpkg)

The most reliable way is to use the **Visual Studio Developer Command Prompt** (or Developer PowerShell), because it sets up the correct environment (MSVC, CMake, nmake/MSBuild).

### 3.1 Open a Developer Command Prompt

1. Open the **Start menu**.  
2. Search for:

   - `Developer Command Prompt for VS 2022`  
     or  
   - `Developer Command Prompt for VS 2026`

   (You can also use **Developer PowerShell for VS** if you prefer PowerShell syntax.)

3. Open it. You’ll see a banner similar to:

   ```text
   Visual Studio 2026 Developer Command Prompt v18.1.1
   ```

Check that `cmake` is available:

```cmd
cmake --version
```

If it prints a version number, you’re good.

---

### 3.2 Go to the project folder

In that Developer Command Prompt:

```cmd
cd C:\Users\<YourUser>\source\repos\SoundcardStrech
```

Adjust the path if you cloned somewhere else.

---

### 3.3 Bootstrap vcpkg (only once per machine)

Run:

```cmd
.\vcpkg\bootstrap-vcpkg.bat
```

This downloads/updates the `vcpkg.exe` tool into the local `vcpkg/` folder.

---

### 3.4 Configure vcpkg environment (manifest mode, x64)

Still in the Developer Command Prompt, set these environment variables:

```cmd
set VCPKG_FEATURE_FLAGS=manifests
set VCPKG_DEFAULT_TRIPLET=x64-windows
set VCPKG_TARGET_TRIPLET=x64-windows
```

- `VCPKG_FEATURE_FLAGS=manifests` tells vcpkg to use **manifest mode** (read dependencies from `vcpkg.json`).  
- The `x64-windows` triplets ensure we consistently build 64-bit.

You will need to re-run at least:

```cmd
set VCPKG_FEATURE_FLAGS=manifests
```

each time you open a fresh Developer Command Prompt and reconfigure the project.

---

### 3.5 Configure the project with CMake (x64)

Now, let CMake generate the build files and let vcpkg install dependencies from `vcpkg.json`:

```cmd
cmake -S . -B build -A x64 -DCMAKE_TOOLCHAIN_FILE="%CD%\vcpkg\scripts\buildsystems\vcpkg.cmake"
```

What this does:

- `-S .` – use the current directory as the source dir.  
- `-B build` – create a `build` directory for generated files.  
- `-A x64` – generate a **64-bit** build (important; vcpkg packages are `x64-windows`).  
- `-DCMAKE_TOOLCHAIN_FILE=...` – tells CMake to integrate with vcpkg.

On the **first run** on a machine, vcpkg will:

- Download its own tools (CMake, 7-Zip, PowerShell Core, etc. if needed).  
- Download/build wxWidgets, PortAudio, Rubber Band, and transitive deps.

This can take several minutes. Subsequent runs are much faster because of caching.

If everything works, you should see output ending with something like:

```text
-- Running vcpkg install
The following packages are already installed: ...
-- Running vcpkg install - done
-- Configuring done
-- Generating done
-- Build files have been written to: C:/Users/<YourUser>/source/repos/SoundcardStrech/build
```

---

### 3.6 Build the executable

Now compile the app in **Debug** mode:

```cmd
cmake --build build --config Debug
```

This drives MSBuild under the hood. If successful, you’ll see a line like:

```text
Soundcard_wav.vcxproj -> C:\Users\<YourUser>\source\repos\SoundcardStrech\build\bin\Debug\Soundcard_wav.exe
```

---

### 3.7 Run the application

From the same prompt:

```cmd
.\build\bin\Debug\Soundcard_wav.exe
```

You should see the wxWidgets GUI window appear.

---

## 4. Next time: quick rebuild

Once CMake has configured the project and the `build/` directory exists, you don’t need to run the full configure step unless:

- you delete the `build/` folder,  
- you edit `CMakeLists.txt`,  
- you add/remove source files in a way that CMake needs to know about.  

For everyday development:

1. Open **Developer Command Prompt for VS**.  
2. Go to the repo:

   ```cmd
   cd C:\Users\<YourUser>\source\repos\SoundcardStrech
   ```

3. Re-enable manifest mode (vcpkg):

   ```cmd
   set VCPKG_FEATURE_FLAGS=manifests
   ```

4. Build:

   ```cmd
   cmake --build build --config Debug
   ```

5. Run:

   ```cmd
   .\build\bin\Debug\Soundcard_wav.exe
   ```

---

## 5. Common problems and fixes

### 5.1 `'cmake' is not recognized as an internal or external command`

Make sure you are **not** using a plain PowerShell / cmd, but the **Developer Command Prompt for Visual Studio** (or Developer PowerShell). Those prompts put the VS CMake on PATH.

If `cmake --version` still fails in the Developer Command Prompt:

- Open **Visual Studio Installer**.  
- Modify your VS installation.  
- Ensure **“C++ CMake tools for Windows”** is checked in **Individual components**.

---

### 5.2 NanoSVG / wxWidgets architecture mismatch

Error example:

```text
Could not find a configuration file for package "NanoSVG" that is compatible with requested version "".
...
NanoSVGConfig.cmake, version: 1.0 (64bit)
```

or something mentioning `NanoSVGConfig.cmake` / wxWidgets and 64-bit.

This usually means:

- vcpkg installed **x64-windows** libraries, but  
- CMake tried to configure a **Win32 (x86)** project.

Fix:

1. Delete any previous build directory:

   ```cmd
   rmdir /S /Q build
   ```

2. Re-run configure **with `-A x64`**:

   ```cmd
   set VCPKG_FEATURE_FLAGS=manifests
   set VCPKG_DEFAULT_TRIPLET=x64-windows
   set VCPKG_TARGET_TRIPLET=x64-windows

   cmake -S . -B build -A x64 -DCMAKE_TOOLCHAIN_FILE="%CD%\vcpkg\scripts\buildsystems\vcpkg.cmake"
   ```

3. Build again:

   ```cmd
   cmake --build build --config Debug
   ```

---

### 5.3 “Could not find wxWidgets / PortAudio / Rubber Band” during configure

If CMake complains that it cannot find wxWidgets / PortAudio / Rubber Band:

1. Ensure you ran CMake with the vcpkg toolchain:

   ```cmd
   cmake -S . -B build -A x64 -DCMAKE_TOOLCHAIN_FILE="%CD%\vcpkg\scripts\buildsystems\vcpkg.cmake"
   ```

2. Ensure manifest mode is enabled:

   ```cmd
   set VCPKG_FEATURE_FLAGS=manifests
   ```

3. If it still fails, explicitly install the dependencies once:

   ```cmd
   .\vcpkg\vcpkg.exe install --triplet x64-windows
   ```

   and then re-run the `cmake -S . -B build ...` step.

---

## 6. Notes and future directions

- The current setup is primarily tested on **Windows x64**.  
- The use of CMake + vcpkg makes it possible to eventually support other platforms (Linux, macOS) by:
  - installing vcpkg on those systems,  
  - ensuring wxWidgets / PortAudio / Rubber Band are available for that platform,  
  - and possibly adapting any Windows-specific parts of the code.
- If you add new `.cpp` or `.h` files under `Soundcard_wav/`, make sure:
  - they are included in the `SC_SOURCES` / `SC_HEADERS` lists in `CMakeLists.txt`, or  
  - you adjust `CMakeLists.txt` so they are picked up automatically.

---

If you follow the steps in sections **2** and **3** on a fresh Windows install with a reasonably recent Visual Studio, you should end up with a working `Soundcard_wav.exe` without manually downloading or configuring any third-party libraries.
