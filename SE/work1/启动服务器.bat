@echo off
REM Photo Server Launcher
echo Starting Photo Server...
echo.

REM Check if exe exists
if exist "build\Desktop_Qt_6_8_3_MSVC2022_64bit-Debug\bin\photo_server.exe" (
    start "" "build\Desktop_Qt_6_8_3_MSVC2022_64bit-Debug\bin\photo_server.exe"
    echo Server started!
    echo.
    echo Open browser and visit:
    echo   http://localhost:6006
    echo.
    timeout /t 3
    exit
) else if exist "build\Desktop_Qt_6_8_3_MSVC2022_64bit-Release\bin\photo_server.exe" (
    start "" "build\Desktop_Qt_6_8_3_MSVC2022_64bit-Release\bin\photo_server.exe"
    echo Server started!
    echo.
    echo Open browser and visit:
    echo   http://localhost:6006
    echo.
    timeout /t 3
    exit
) else (
    echo ERROR: photo_server.exe not found!
    echo Please compile the project first in Qt Creator.
    echo.
    pause
    exit /b 1
)



