/* ============================================================
 * test_tensor_invalid.c — Invalid tensor shape mismatch
 * ============================================================ */

// Example 3: Should fail - shape mismatch
tensor A[2][3];
tensor B[3][2];
tensor C[2][3];

int main() {
    C = A + B;
    return 0;
}
