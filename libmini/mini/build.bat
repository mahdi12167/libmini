@set BUILDDIR=%CD%
@set PROJECT=%1%

@IF "%PROJECT%" == "" ( set PROJECT=libMini )

@IF EXIST "%ProgramFiles(x86)%" ( set "PFDIR=%ProgramFiles(x86)%" ) ELSE ( set "PFDIR=%ProgramFiles%" )
@call "%PFDIR%\Microsoft Visual Studio 8\Common7\Tools\vsvars32.bat"

@echo building DEBUG configuration
@echo ...
@devenv %BUILDDIR%\viewer-vc8.sln /Build debug /project %PROJECT%

@echo building RELEASE configuration
@echo ...
@devenv %BUILDDIR%\viewer-vc8.sln /Build release /project %PROJECT%
