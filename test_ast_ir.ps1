# PowerShell script to test AST and IR implementation
# Sets up environment and runs the demo

Write-Host "========================================" -ForegroundColor Cyan
Write-Host "AST and IR Testing Script" -ForegroundColor Cyan
Write-Host "========================================" -ForegroundColor Cyan
Write-Host ""

# Set PATH for MSYS2 tools
$env:Path = "C:\msys64\usr\bin;C:\msys64\mingw64\bin;" + $env:Path

Write-Host "Step 1: Cleaning previous build..." -ForegroundColor Yellow
make clean

Write-Host ""
Write-Host "Step 2: Building all components..." -ForegroundColor Yellow
make

if ($LASTEXITCODE -ne 0) {
    Write-Host "Build failed! Exiting..." -ForegroundColor Red
    exit 1
}

Write-Host ""
Write-Host "========================================" -ForegroundColor Green
Write-Host "Running AST/IR Demo Program" -ForegroundColor Green
Write-Host "========================================" -ForegroundColor Green
Write-Host ""

./ast_ir_demo

Write-Host ""
Write-Host "========================================" -ForegroundColor Green
Write-Host "Demo completed!" -ForegroundColor Green
Write-Host "========================================" -ForegroundColor Green
