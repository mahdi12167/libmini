if exist ..\mini (
   pushd ..\mini
   call build.bat
   popd
)

qmake
qmake -tp vc

nmake release
