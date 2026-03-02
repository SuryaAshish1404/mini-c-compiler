/* ============================================================
 * test_tensor_valid.c — Valid tensor operations
 * ============================================================ */

// Example 1: Basic tensor addition
tensor A[2][2];
tensor B[2][2];
tensor C[2][2];

int main() {
    C = A + B;
    return 0;
}
