int add(int x, int y) {
    return x + y;
}

int multiply(int a, int b) {
    int result;
    result = a * b;
    return result;
}

int main() {
    int x;
    int y;
    int sum;
    int product;
    
    x = 10;
    y = 20;
    
    sum = add(x, y);
    product = multiply(x, y);
    
    if (sum > product) {
        return sum;
    }
    
    return product;
}
