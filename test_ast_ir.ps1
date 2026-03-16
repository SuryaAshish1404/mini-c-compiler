# quick helper to rebuild and run demo
$env:Path = "C:\msys64\usr\bin;C:\msys64\mingw64\bin;" + $env:Path

Write-Host "clean"
make clean

Write-Host "build"
make

if ($LASTEXITCODE -ne 0) {
    Write-Host "build failed"
    exit 1
}

Write-Host "run demo"
./ast_ir_demo

if ($LASTEXITCODE -ne 0) {
    Write-Host "demo failed"
    exit 1
}

Write-Host "done"
