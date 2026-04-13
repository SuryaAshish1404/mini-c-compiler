Write-Host "=====================================" -ForegroundColor Cyan
Write-Host "Mini C Compiler - Optimization Tests" -ForegroundColor Cyan
Write-Host "=====================================" -ForegroundColor Cyan
Write-Host ""

Write-Host "[Test 1] Constant Folding" -ForegroundColor Yellow
Write-Host "Testing: 10+5, 20*3, 100-25, 50/2" -ForegroundColor Gray
./mini_compiler test/test_constant_folding.c
Write-Host ""

Write-Host "[Test 2] Common Subexpression Elimination" -ForegroundColor Yellow
Write-Host "Testing: Multiple a+b and x*2 computations" -ForegroundColor Gray
./mini_compiler test/test_cse_demo.c
Write-Host ""

Write-Host "[Test 3] Dead Code Elimination" -ForegroundColor Yellow
Write-Host "Testing: Removal of unused variables" -ForegroundColor Gray
./mini_compiler test/test_dead_code.c
Write-Host ""

Write-Host "[Test 4] All Optimizations Combined" -ForegroundColor Yellow
Write-Host "Testing: Constant folding + CSE + DCE" -ForegroundColor Gray
./mini_compiler test/test_all_optimizations.c
Write-Host ""

Write-Host "[Test 5] Assembly Generation" -ForegroundColor Yellow
Write-Host "Testing: x86-64 assembly with functions" -ForegroundColor Gray
./mini_compiler test/test_assembly_gen.c output.c test_assembly.s
if (Test-Path test_assembly.s) {
    Write-Host "Assembly file generated: test_assembly.s" -ForegroundColor Green
    Write-Host "First 20 lines:" -ForegroundColor Gray
    Get-Content test_assembly.s -Head 20
} else {
    Write-Host "Assembly file not generated" -ForegroundColor Red
}
Write-Host ""

Write-Host "=====================================" -ForegroundColor Cyan
Write-Host "Tests Complete!" -ForegroundColor Cyan
Write-Host "=====================================" -ForegroundColor Cyan
