/* ============================================================
 * test_tensor_valid2.c — Valid tensor multiplication
 * ============================================================ */

// Example 2: Tensor element-wise multiplication
tensor X[3][3];
tensor Y[3][3];
tensor Z[3][3];

int main() {
    Z = X * Y;
    return 0;
}
