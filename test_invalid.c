/* ============================================================
 * test_invalid.c — Invalid syntax examples for error reporting
 * ============================================================ */

// Missing semicolon on variable declaration
int bad_var

// Missing closing parenthesis in if-statement
int main() {
    int x = 10;

    if (x > 10 {
        x = 5;
    }

    return 0;
}
