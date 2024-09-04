# 设置变量
$folder = "D:\C++\Projects\PathTracer\EngineRuntime\AssetSources\Shaders"
$compiler = "D:\Vulkan\SDK\1.3.268.0\Bin\glslangValidator.exe"
$extensions = @("*.frag", "*.vert", "*.rgen", "*.rmiss", "*.rchit")

# 获取所有匹配的文件
$source_folder = Join-Path $folder "Sources"
$files = Get-ChildItem -Path $source_folder -Recurse -Include $extensions

# 遍历每个文件并调用编译器
foreach ($file in $files) {
    $filename = $file.Name
    $outputFile = "$folder\Binarys\$filename.spv"
    Write-Host "$compiler --target-env vulkan1.2 -g $file -o $outputFile"
    & $compiler --target-env vulkan1.2 -g $file.FullName -o $outputFile
    Write-Host ""
}

# Read-Host -Prompt "Press any key to continue"
