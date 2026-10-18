param(
    [switch]$Warnings,
    [string]$OutputName = 'TV36.exe'
)

$ErrorActionPreference = 'Stop'
if ([IO.Path]::GetFileName($OutputName) -ne $OutputName -or
    [IO.Path]::GetExtension($OutputName) -ne '.exe') {
    throw 'OutputName must be an executable filename, for example TV36-updated.exe.'
}
$compiler = Get-Command g++.exe -ErrorAction SilentlyContinue
if (-not $compiler) {
    throw 'g++.exe was not found in PATH. Add your MSYS2 UCRT64 bin directory and reopen the terminal.'
}

$outputDirectory = Join-Path $PSScriptRoot 'build'
New-Item -ItemType Directory -Path $outputDirectory -Force | Out-Null
$sources = @((Join-Path $PSScriptRoot 'main.cpp'))
foreach ($directory in @('Entities', 'UseCases', 'Utils')) {
    $sources += Get-ChildItem -LiteralPath (Join-Path $PSScriptRoot $directory) -Filter '*.cpp' -Recurse |
        Sort-Object FullName | Select-Object -ExpandProperty FullName
}

$compilerArguments = @('-std=c++17', '-g')
if ($Warnings) { $compilerArguments += @('-Wall', '-Wextra') }
$compilerArguments += $sources
$compilerArguments += @('-I', $PSScriptRoot, '-o', (Join-Path $outputDirectory $OutputName))

Write-Host "[BUILD] Compiling $($sources.Count) source files..."
& $compiler.Source @compilerArguments
if ($LASTEXITCODE -ne 0) {
    Write-Error 'Build failed. Read the compiler diagnostics above.'
    exit 1
}
Write-Host "[SUCCESS] build\$OutputName"
