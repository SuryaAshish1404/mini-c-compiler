/* ============================================================
 * test_invalid_declarations.c — Declaration-related syntax errors
 * ============================================================ */

// Missing type keyword
value = 10;

int main() {
    // Redeclaration in same scope
    int count = 0;
    int count = 1;

    // Initialization missing expression
    int broken = ;

    // Array declaration missing size and initializer
    int nums[];

    return 0;
}
