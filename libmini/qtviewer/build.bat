@rem if the batch file is not run from the msvc command prompt
@rem try to find the vcvars batch file to set the msvc environment
@set vcbat="C:\Program Files (x86)\Microsoft Visual Studio 10.0\VC\bin\vcvars32.bat"
@if exist %vcbat% call %vcbat%

qmake
@qmake -tp vc
nmake release

@copy release\qtviewer.exe .
@copy ..\..\gdal\bin\gdal*.dll .
