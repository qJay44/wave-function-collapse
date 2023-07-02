:: Build
if not exist Build\Debug mkdir Build\Debug
cd Build\Debug
cmake -S ..\..\ -B . -G"MinGW Makefiles" -D CMAKE_EXPORT_COMPILE_COMMANDS=ON
C:\MinGW\mingw64\bin\mingw32-make.exe

:: Start
cd ..\..\
Build\Debug\MyProject.exe
