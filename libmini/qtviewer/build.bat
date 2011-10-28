# windows build batch file

if exist ..\mini (
   cd ..\mini
   build.bat
   cd -
)

qmake
nmake release
