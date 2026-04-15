@echo off
setlocal
cd /d %~dp0

rem Clean generated headers to avoid stale cross-arch files
del /q test\*.hpp 2>nul
if exist test\selfref_out rd /s /q test\selfref_out 2>nul
if exist test\bug19_out rd /s /q test\bug19_out 2>nul
if exist test\deltagen_out rd /s /q test\deltagen_out 2>nul

echo === x64 ===
call "C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvars64.bat" >nul 2>&1
cmake -B build -G Ninja -DCMAKE_BUILD_TYPE=Release
if %errorlevel% neq 0 exit /b %errorlevel%
ninja -C build -t clean >nul 2>&1
cmake --build build
if %errorlevel% neq 0 exit /b %errorlevel%
ctest --test-dir build -C Release --output-on-failure
if %errorlevel% neq 0 exit /b %errorlevel%
echo.
echo === x64 header verification ===
python test\verify_headers.py
if %errorlevel% neq 0 exit /b %errorlevel%

echo.
echo === x86 ===
del /q test\*.hpp 2>nul
call "C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvars32.bat" >nul 2>&1
cmake -B build32 -G Ninja -DCMAKE_BUILD_TYPE=Release -DVCPKG_TARGET_TRIPLET=x86-windows
if %errorlevel% neq 0 exit /b %errorlevel%
cmake --build build32
if %errorlevel% neq 0 exit /b %errorlevel%
ctest --test-dir build32 -C Release --output-on-failure
if %errorlevel% neq 0 exit /b %errorlevel%

echo.
echo === All tests passed on both architectures ===
