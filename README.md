git clone https://github.com/TimothyLeonerd/SoundcardStrech.git
cd SoundcardStrech

git submodule update --init --recursive
.\vcpkg\bootstrap-vcpkg.bat
$env:VCPKG_FEATURE_FLAGS="manifests"

cmake -S . -B build -DCMAKE_TOOLCHAIN_FILE="$PWD/vcpkg/scripts/buildsystems/vcpkg.cmake"
cmake --build build --config Debug

.\build\bin\Debug\Soundcard_wav.exe
