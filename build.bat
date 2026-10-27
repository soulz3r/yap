@ECHO OFF
SETLOCAL EnableDelayedExpansion

REM Get a list of all .c files inside src.
SET "cFilenames="
FOR /R src %%f IN (*.c) DO (
    SET cFilenames=!cFilenames! "%%f"
)

SET "appName=yap"

REM Header search paths
SET "includeFlags=-Iinclude -Isrc"

IF NOT EXIST .\bin MKDIR .\bin

clang !cFilenames! %compilerFlags% -o .\bin\%appName%.exe %includeFlags%

IF ERRORLEVEL 1 (
    ECHO.
    ECHO %appName% build failed.
    EXIT /B 1
)

ECHO.
ECHO %appName% build succeeded.
EXIT /B 0

