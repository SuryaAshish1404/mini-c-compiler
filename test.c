/* ============================================================
 * test.c — Sample input for the Mini C Compiler
 *
 * This file contains BOTH valid and invalid syntax sections.
 * The invalid section is commented-out by default so the parser
 * can demonstrate a successful run.  Uncomment it to test error
 * reporting.
 * ============================================================ */

// ----- Valid Syntax -----

// Global variable declarations
int globalCount;
float pi = 3.14;

// Function with parameters
int add(int a, int b) {
    return a + b;
}

// Function demonstrating control flow
int main() {
    // Variable declarations with initialization
    int x = 10;
    int y = 20;
    int result;

    // Arithmetic expression
    result = x + y * 2;

    // If-else statement
    if (result > 50) {
        result = result - 10;
    } else {
        result = result + 10;
    }

    // While loop
    int counter = 0;
    while (counter < 5) {
        counter++;
    }

    // For loop
    int sum = 0;
    for (int i = 0; i < 10; i++) {
        sum += i;
    }

    // Nested if
    if (x == 10 && y == 20) {
        if (sum > 0 || counter != 0) {
            result = sum + counter;
        }
    }

    // Function call
    int total = add(x, y);

    // Unary operators
    int neg = -x;
    int flag = !0;

    // Compound assignment
    total += 100;
    total -= 50;
    total *= 2;
    total /= 3;

    return 0;
}

/* ----- Invalid Syntax (uncomment to test error reporting) -----

// Missing semicolon
int bad_var

// Missing closing parenthesis
if (x > 10 {
    x = 5;
}

// Invalid operator
int z = x @ y;

// Missing closing brace
int broken() {
    int a = 1;

----- End of Invalid Syntax ----- */
