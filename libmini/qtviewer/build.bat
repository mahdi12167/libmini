; windows build batch file

if exist ..\mini (
   pushd ..\mini
   build.bat
   popd
)

qmake
nmake release
