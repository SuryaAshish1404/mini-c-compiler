/* ============================================================
 * test_invalid_control.c — Control flow syntax errors
 * ============================================================ */

int main() {
    int n = 5;

    // If condition missing closing parenthesis
    if (n > 0 {
        n--;
    }

    // Else without matching if in current scope
    else {
        n++;
    }

    // While loop missing closing brace
    while (n > 0) {
        n--;

    // For loop missing initializer semicolon
    for (int i = 0 i < 3; i++) {
        n += i;
    }

    return 0;
}
