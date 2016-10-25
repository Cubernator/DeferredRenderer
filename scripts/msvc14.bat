@echo off
mkdir ..\build-msvc14
cd ..\build-msvc14

cmake ../ -G "Visual Studio 14 2015"

pause