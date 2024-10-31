@REM remove build folder
rmdir /s /q build

@REM remove other files if outside build folder, such as visual studio files
rmdir /s /q CMakeFiles
rm main.exe
rm Makefile
rm CMakeCache.txt
rm cmake_install.cmake
rm *.vcxproj
rm *.filters
rm *.sln
