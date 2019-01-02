Add-Type -AssemblyName System.IO.Compression.FileSystem
function Unzip
{
    param([string]$zipfile, [string]$outpath)

    [System.IO.Compression.ZipFile]::ExtractToDirectory($zipfile, $outpath)
}

$CURRENT_DIR_PATH = (Get-Item -Path ".\" -Verbose).FullName
$BUILD_DIR = Join-Path $CURRENT_DIR_PATH build

#prepare directories
Write-Host "Preparing directories"

$exists = Test-Path build
if ($exists -eq $false) {
    mkdir build
}
cd build

$exists = Test-Path $env:RELEASE_DIR
if ($exists -eq $false) {
    mkdir $env:RELEASE_DIR
}

$exists = Test-Path python
if ($exists -eq $true) {
    Remove-Item python -recurse
}

$exists = Test-Path "$env:CPPUNIT_PATH"
if ($exists -eq $true) {
    Remove-Item "$env:CPPUNIT_PATH" -recurse
}

$exists = Test-Path "$env:CRYPTO_PACKAGE_PATH"
if ($exists -eq $true) {
    Remove-Item "$env:CRYPTO_PACKAGE_PATH" -recurse
}

mkdir python

Write-Host "Preparing directories - OK"

Write-Host "Downloading needed tools and dependencies"

[Net.ServicePointManager]::SecurityProtocol = "tls12, tls11, tls"

$exists = Test-Path "$env:CRYPTO_PACKAGE_NAME"
if ($exists -eq $false) {
    $source = "https://github.com/disig/SoftHSM2-AppVeyor/raw/master/$env:PACKAGE_VERSION_NAME/$env:CRYPTO_PACKAGE"
    Invoke-WebRequest $source -OutFile $env:CRYPTO_PACKAGE
}

$exists = Test-Path "$env:CPPUNIT_PACKAGE"
if ($exists -eq $false) {
    $source = "https://github.com/disig/SoftHSM2-AppVeyor/raw/master/$env:CPPUNIT_VERSION_NAME/$env:CPPUNIT_PACKAGE"
    Invoke-WebRequest $source -OutFile $env:CPPUNIT_PACKAGE
}

Write-Host "Downloading needed tools and dependencies - OK"

Write-Host "Extracting ..."  
Unzip "$BUILD_DIR/$env:CRYPTO_PACKAGE" "$BUILD_DIR"

Unzip "$BUILD_DIR/$env:CPPUNIT_PACKAGE" "$BUILD_DIR"

dir 

dir "$env:PYTHON_PATH"
dir "$env:CRYPTO_PACKAGE_PATH"
dir "$env:CPPUNIT_PATH"

cd $CURRENT_DIR_PATH
