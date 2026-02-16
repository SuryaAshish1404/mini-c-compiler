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

// Additional global helpers
float mix(float lhs, float rhs) {
    float total = lhs + rhs;
    return total / 2.0;
}

void spin_counter() {
    int loops = 3;
    while (loops > 0) {
        loops--;
    }
    return;
}

// Function with parameters
int add(int leftOperand, int rightOperand) {
    return leftOperand + rightOperand;
}

int max_pair(int leftValue, int rightValue) {
    int bigger;
    if (leftValue > rightValue) {
        bigger = leftValue;
    } else {
        bigger = rightValue;
    }
    return bigger;
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

    // Nested block scope
    {
        int shadow = 5;
        result += shadow;
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

    // Function calls
    int total = add(x, y);
    int bigger = max_pair(total, result);
    float midpoint = mix(pi, 2.5);
    spin_counter();

    // Unary operators
    int neg = -x;
    int flag = !0;

    // Compound assignment
    total += 100;
    total -= 50;
    total *= 2;
    total /= 3;

    int midpointRounded = midpoint;
    bigger += midpointRounded;

    return 0;
}
