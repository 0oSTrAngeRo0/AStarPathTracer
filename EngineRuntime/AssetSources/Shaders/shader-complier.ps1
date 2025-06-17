$min_ps_versoin = 5
if ($PSVersionTable.PSVersion.Major -lt $min_ps_versoin) {
    # Display a warning message. You can use Write-Warning, Write-Host or Write-Error depending on the scenario.
    # Here we use Write-Warning (yellow warning) and include the current version number in the message.
    Write-Error "Current PowerShell version is [$PSVersionTable.PSVersion], which is less than [$min_ps_versoin]. Please upgrade to PowerShell [$min_ps_versoin] or higher before running this script."
    # If you want to stop execution when the version is too low, you can exit the script:
    exit 1
}

# Set variables
$folder = "./"
$compiler = "glslangValidator.exe"
$extensions = @("*.frag", "*.vert", "*.rgen", "*.rmiss", "*.rchit")

# Get all matching files
$source_folder = Join-Path $folder "Sources"
if (-not (Test-Path -Path $source_folder -PathType Container)) {
    Write-Error "Sources folder: [$source_folder] is not exists."
    exit 2
}
$files = Get-ChildItem -Path $source_folder -Recurse -Include $extensions

# Iterate over each file and invoke the compiler
foreach ($file in $files) {
    $filename = $file.Name
    $binarys_folder = Join-Path (Resolve-Path $folder) "Binarys"
    if (-not (Test-Path -Path $binarys_folder -PathType Container)) {
        try {
            New-Item -Path $binarys_folder -ItemType Directory -Force | Out-Null
            Write-Host "Folder created：[$binarys_folder]"
        } catch {
            Write-Error "Failed to create folder：[$binarys_folder]."
            Write-Error "Error message：$_"
        }
    }
    $output_file = Join-Path $binarys_folder "$filename.spv"
    Write-Host "Run glslang command: [$compiler --target-env vulkan1.3 -g $file -o $output_file]"
    & $compiler --target-env vulkan1.3 -g $file.FullName -o $output_file
    Write-Host ""
}

# Read-Host -Prompt "Press any key to continue"
