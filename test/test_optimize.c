int main() {
    int a;
    int b;
    int c;
    int unused;
    
    // Constant folding opportunities
    a = 5 + 3;           // Should fold to 8
    b = 10 * 2;          // Should fold to 20
    c = a + b;
    
    // Dead code - unused variable
    unused = 100;        // Should be eliminated
    
    // More constant folding
    int result;
    result = 15 - 5;     // Should fold to 10
    
    return c + result;
}
