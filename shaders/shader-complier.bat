@echo off

set "folder=D:\C++\Projects\PathTracer\shaders"
set "complier=D:\Vulkan\SDK\1.3.268.0\Bin\glslc.exe"
set "extension=(*.frag *.vert)"


for /r "%folder%" %%i in %extension% do (
    echo "%complier% %%i -o %%i.spv"
    %complier% %%i -o %%i.spv
)

pause
