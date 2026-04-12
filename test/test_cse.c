int main() {
    int a;
    int b;
    int x;
    int y;
    int z;
    
    a = 5;
    b = 10;
    
    // Common subexpression: a + b appears twice
    x = a + b;
    y = a + b;  // Should reuse result from x = a + b
    
    // Another common subexpression
    z = x * 2;
    int w;
    w = x * 2;  // Should reuse result from z = x * 2
    
    return x + y + z + w;
}
