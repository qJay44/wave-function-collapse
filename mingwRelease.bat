:: Build
if not exist Build\Release mkdir Build\Release
cd Build\Release
cmake -S ..\..\ -B . -G"MinGW Makefiles" -D CMAKE_EXPORT_COMPILE_COMMANDS=ON -D CMAKE_BUILD_TYPE=Release
C:\MinGW\mingw32\bin\mingw32-make.exe

:: Start
cd ..\..\
Build\Release\MyProject.exe
