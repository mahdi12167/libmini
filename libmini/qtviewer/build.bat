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

qmake
@qmake -tp vc
nmake release

@copy release\qtviewer.exe .
