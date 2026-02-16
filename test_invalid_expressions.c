/* ============================================================
 * test_invalid_expressions.c — Expression grammar errors
 * ============================================================ */

int main() {
    int a = 5;
    int b = 10;

    // Missing right operand
    int sum = a + ;

    // Chained comparison not allowed (lacks parentheses and operators)
    if (a < b < 20) {
        b = 0;
    }

    // Assignment in condition missing expression
    while ((a = )) {
        a++;
    }

    // Invalid logical operator sequence
    if (a && || b) {
        b--;
    }

    return 0;
}
