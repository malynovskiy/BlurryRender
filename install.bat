@echo off

pushd .

::set SolutionDir=%1
::set Target=%2
::
::echo Copying shaders to bin folder
::mkdir %SolutionDir%bin\%Target%\shaders
::robocopy %SolutionDir%BlurryRender\shaders %SolutionDir%bin\%Target%\shaders /NFL /NDL /NJH /NJS /nc /ns /np
::echo Copying resources to bin folder
::mkdir %SolutionDir%bin\%Target%\resources
::robocopy  %SolutionDir%BlurryRender\resources %SolutionDir%bin\%Target%\resources /E /NFL /NDL /NJH /NJS /nc /ns /np

where vcpkg >nul 2>nul
if %ERRORLEVEL% neq 0 (
    echo vcpkg is not installed.
    exit /b 1
)
echo vcpkg is installed.

vcpkg install stb:x64-windows
vcpkg install glm:x64-windows
vcpkg install glfw3:x64-windows
vcpkg install glad:x64-windows
vcpkg install assimp:x64-windows

vcpkg integrate install

popd
