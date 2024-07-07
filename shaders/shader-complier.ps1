# 设置变量
$folder = "D:\C++\Projects\PathTracer\shaders"
$compiler = "D:\Vulkan\SDK\1.3.268.0\Bin\glslangValidator.exe"
$extensions = @("*.frag", "*.vert", "*.rgen", "*.rmiss", "*.rchit")

# 获取所有匹配的文件
$files = Get-ChildItem -Path $folder -Recurse -Include $extensions

# 遍历每个文件并调用编译器
foreach ($file in $files) {
    $outputFile = "$($file.FullName).spv"
    Write-Host "$compiler --target-env vulkan1.2 $file -o $outputFile"
    & $compiler --target-env vulkan1.2 $file.FullName -o $outputFile
    Write-Host ""
}

Read-Host -Prompt "Press any key to continue"
