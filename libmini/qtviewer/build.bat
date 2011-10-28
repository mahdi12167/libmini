# windows build batch file

if exist ..\mini ..\mini\build.bat
qmake
nmake release
