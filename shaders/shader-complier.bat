@echo off

set "folder=D:\C++\Projects\PathTracer\shaders"
set "complier=D:\Vulkan\SDK\1.3.268.0\Bin\glslangValidator.exe"
set "extension=(*.frag *.vert *.rgen *.rmiss *.rchit)"


for /r "%folder%" %%i in %extension% do (
    echo "%complier% --target-env vulkan1.2 %%i -o %%i.spv"
    %complier% --target-env vulkan1.2 %%i -o %%i.spv
    echo:
)
