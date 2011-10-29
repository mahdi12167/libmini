if exist ..\mini (
   pushd ..\mini
   call build.bat
   popd
)

qmake
nmake release
