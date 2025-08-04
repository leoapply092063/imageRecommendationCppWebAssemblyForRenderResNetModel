@echo off

REM Build script for the image recommendation system (Windows)

echo Building image recommendation system...

REM Create build directory
if not exist build mkdir build
cd build

REM Configure with CMake
echo Configuring with CMake...
cmake ..

REM Build the project
echo Building the project...
cmake --build . --config Release

REM Copy executable to project root
copy Release\feature_extractor.exe ..\

echo Build completed successfully!
echo You can now run: npm start

cd .. 