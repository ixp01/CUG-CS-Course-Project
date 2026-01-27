@echo off
REM Photo Server Launcher with Browser
echo Starting Photo Server...
echo.

REM Find and start exe
if exist "build\Desktop_Qt_6_8_3_MSVC2022_64bit-Debug\bin\photo_server.exe" (
    start "" "build\Desktop_Qt_6_8_3_MSVC2022_64bit-Debug\bin\photo_server.exe"
) else if exist "build\Desktop_Qt_6_8_3_MSVC2022_64bit-Release\bin\photo_server.exe" (
    start "" "build\Desktop_Qt_6_8_3_MSVC2022_64bit-Release\bin\photo_server.exe"
) else (
    echo ERROR: photo_server.exe not found!
    pause
    exit /b 1
)

echo Server started!
echo Waiting for server to initialize...
timeout /t 2 >nul

REM Open browser
echo Opening browser...
start http://localhost:6006

echo.
echo Photo Server is running!
echo Close the server window to stop the service.
echo.
timeout /t 3
exit



