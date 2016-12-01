@ECHO OFF

IF "%_PROJ_%"=="" SET _PROJ_=AntiScreenLocker.sln
IF "%_CONF_%"=="" SET _CONF_=Release

:: Set here version of VS
:: Visual Studio 2012 - SET _VS_VER=11
:: Visual Studio 2013 - SET _VS_VER=12
:: Visual Studio 2015 - SET _VS_VER=14
IF "%_VS_VER%"=="" SET _VS_VER=14

echo CALL :GetPathToVisualStudio %_VS_VER%.0
CALL :GetPathToVisualStudio %_VS_VER%.0
ECHO Path to VS: %_PATH_TO_VS%

SET _ENV_BAT_="%_PATH_TO_VS%..\..\VC\vcvarsall.bat"

IF NOT EXIST %_ENV_BAT_% (
  ECHO Visual Studio %_VS_VER% not found! Set another VS version by env var _VS_VER.
  EXIT /B 1
)

CALL %_ENV_BAT_% 
pushd "%~dp0"
msbuild %_PROJ_% /property:Configuration=%_CONF_% /m /flp:logfile=%_PROJ_%.build.log
popd

GOTO END

:GetPathToVisualStudio
CALL :GetPathToVisualStudioHelper HKLM\SOFTWARE\Microsoft\VisualStudio %1 > NUL 2>&1
IF ERRORLEVEL 1 CALL :GetPathToVisualStudioHelper HKLM\SOFTWARE\Wow6432Node\Microsoft\VisualStudio %1 > NUL 2>&1
EXIT /B 0

:GetPathToVisualStudioHelper
FOR /F "tokens=1,2*" %%i IN ('reg query "%1\%2" /v "InstallDir"') DO (
        if "%%i"=="InstallDir" (
                SET "_PATH_TO_VS=%%k"
        )
)
IF "%_PATH_TO_VS%"=="" EXIT /B 1
EXIT /B 0
  
:END
