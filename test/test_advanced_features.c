// Comprehensive test for all advanced compiler features
// Tests: CFG, DAG, Heap Allocation, Stack Frames, Register Allocation

int factorial(int n) {
    // Tests: CFG with branches, register allocation with multiple live vars
    if (n <= 1) {
        return 1;
    }
    return n * factorial(n - 1);
}

int fibonacci(int n) {
    // Tests: CFG with multiple branches
    if (n <= 0) {
        return 0;
    }
    if (n == 1) {
        return 1;
    }
    return fibonacci(n - 1) + fibonacci(n - 2);
}

int compute_expression(int a, int b, int c) {
    // Tests: DAG deduplication - common subexpressions
    int x = a + b;      // First computation of a+b
    int y = a + b;      // Duplicate - should be deduplicated in DAG
    int z = x * y;      // Uses both x and y
    
    // Tests: Constant folding
    int const1 = 10 + 5;        // Should fold to 15
    int const2 = 20 * 3;        // Should fold to 60
    int const3 = 100 - 25;      // Should fold to 75
    
    // Tests: Register allocation with many live variables
    int result = z + const1 + const2 + const3 + c;
    return result;
}

int complex_control_flow(int x) {
    // Tests: Complex CFG with nested branches and loops
    int result = 0;
    int i = 0;
    
    if (x > 10) {
        // Tests: Multiple basic blocks with loop (back edges)
        while (i < x) {
            if (i == 5) {
                result = result + i * 2;
            } else {
                result = result + i;
            }
            i = i + 1;
        }
    } else {
        // Alternative path
        result = x * 2;
    }
    
    return result;
}

int stress_register_allocation(int a, int b, int c, int d, int e, int f) {
    // Tests: Register allocation with many live variables
    // Forces spilling with 13 physical registers (K=13)
    int v1 = a + b;
    int v2 = c + d;
    int v3 = e + f;
    int v4 = v1 + v2;
    int v5 = v2 + v3;
    int v6 = v3 + v1;
    int v7 = v4 + v5;
    int v8 = v5 + v6;
    int v9 = v6 + v4;
    int v10 = v7 + v8;
    int v11 = v8 + v9;
    int v12 = v9 + v7;
    int v13 = v10 + v11;
    int v14 = v11 + v12;  // Should cause spilling
    int v15 = v12 + v13;  // More spilling
    int v16 = v13 + v14;  // Even more spilling
    
    return v14 + v15 + v16;
}

int nested_loops(int n) {
    // Tests: CFG with nested loops (multiple back edges)
    int sum = 0;
    int i = 0;
    while (i < n) {
        int j = 0;
        while (j < i) {
            sum = sum + i + j;
            j = j + 1;
        }
        i = i + 1;
    }
    return sum;
}

int main() {
    // Tests: Stack frame with proper ABI, function calls
    int fact5 = factorial(5);
    int fib7 = fibonacci(7);
    
    // Tests: DAG and register allocation
    int expr_result = compute_expression(5, 10, 15);
    
    // Tests: Complex CFG with loops
    int flow_result = complex_control_flow(15);
    
    // Tests: Register spilling
    int stress_result = stress_register_allocation(1, 2, 3, 4, 5, 6);
    
    // Tests: Nested loops (complex CFG)
    int nested_result = nested_loops(8);
    
    // Final computation with many live variables
    int temp1 = fact5 + fib7;
    int temp2 = expr_result + flow_result;
    int temp3 = stress_result + nested_result;
    int final_result = temp1 + temp2 + temp3;
    
    return final_result;
}
