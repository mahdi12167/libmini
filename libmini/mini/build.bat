; windows build batch file

cmake -G "NMake Makefiles" -DBUILD_MINI_SFX=ON -DCMAKE_BUILD_TYPE=RELEASE
nmake
