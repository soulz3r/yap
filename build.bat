REM Build script for testbed
@ECHO OFF
SetLocal EnableDelayedExpansion

REM Get a list of all the .c files.
SET cFilenames=
FOR /R %%f in (*.c) do (
    SET cFilenames=!cFilenames! %%f
)

REM echo "Files:" %cFilenames%

SET assembly=testbed
SET compilerFlags=-g
REM -Wall -Werror

SET includeFlags=-Isrc -I../engine/src/
SET defines=-D_DEBUG

ECHO "Building %assembly%..."
clang %cFilenames% %compilerFlags% -o ../bin/%assembly%.exe %defines% %includeFlags% %linkerFlags%

IF ERRORLEVEL 1 (
    ECHO.
    ECHO Testbed build failed.
    EXIT /B 1
)

ECHO.
ECHO Testbed build succeeded.
EXIT /B 0


