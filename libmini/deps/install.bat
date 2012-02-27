@rem if the batch file is not run from the msvc command prompt
@rem  try to find the vcvars batch file to set the msvc environment
@set vsdir=%VS80COMNTOOLS%\..\..
@set vcbat="%vsdir%\VC\bin\vcvars32.bat"
@if exist %vcbat% call %vcbat%
@set vsdir=%VS90COMNTOOLS%\..\..
@set vcbat="%vsdir%\VC\bin\vcvars32.bat"
@if exist %vcbat% call %vcbat%
@set vsdir=%VS100COMNTOOLS%\..\..
@set vcbat="%vsdir%\VC\bin\vcvars32.bat"
@if exist %vcbat% call %vcbat%

@set GDAL_DEST=%CD%\..\..\gdal

@cd gdal
nmake /f makefile.vc MSVC_VER=1600 GDAL_HOME=%GDAL_DEST%
nmake /f makefile.vc MSVC_VER=1600 GDAL_HOME=%GDAL_DEST% staticlib
copy gdal.lib %GDAL_DEST%
copy port\*.h %GDAL_DEST%
copy gcore\*.h %GDAL_DEST%
copy ogr\*.h %GDAL_DEST%
@cd ..
