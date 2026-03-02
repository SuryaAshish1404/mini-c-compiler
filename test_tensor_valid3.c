/* ============================================================
 * test_tensor_valid3.c — Multiple tensor operations
 * ============================================================ */

tensor A[2][3];
tensor B[2][3];
tensor C[2][3];
tensor D[2][3];

int main() {
    C = A + B;
    D = A - B;
    return 0;
}
