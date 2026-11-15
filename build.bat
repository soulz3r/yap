@ECHO OFF
SETLOCAL EnableDelayedExpansion

SET "appName=yap"
SET "compiler=clang"

REM Collect all .c files inside src.
SET "cFilenames="
FOR /R src %%f IN (*.c) DO (
    SET "cFilenames=!cFilenames! "%%f""
)

REM Header search paths.
SET "includeFlags=-Iinclude -Isrc"

REM Create bin directory if it doesn't exist.
IF NOT EXIST bin MKDIR bin

ECHO Building %appName%...

%compiler% -g -O0 -Wall -std=c23 -DYAP_PLATFORM_WINDOWS %cFilenames% %includeFlags% %compilerFlags% -o "bin\%appName%.exe"

IF ERRORLEVEL 1 (
    ECHO.
    ECHO %appName% build failed.
    EXIT /B 1
)

ECHO.
ECHO %appName% build succeeded.
EXIT /B 0
