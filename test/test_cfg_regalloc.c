int fibonacci(int n) {
    int a = 0;
    int b = 1;
    int temp;
    int i = 0;
    
    while (i < n) {
        temp = a + b;
        a = b;
        b = temp;
        i++;
    }
    
    return a;
}

int factorial(int num) {
    int result = 1;
    int counter = 1;
    
    while (counter <= num) {
        result = result * counter;
        counter++;
    }
    
    return result;
}

int main() {
    int x = 5;
    int y = 10;
    int z;
    
    if (x > 3) {
        z = x + y;
    } else {
        z = x - y;
    }
    
    int fib_result = fibonacci(x);
    int fact_result = factorial(y);
    
    int total = fib_result + fact_result + z;
    
    int loop_sum = 0;
    for (int i = 0; i < 10; i++) {
        loop_sum = loop_sum + i;
    }
    
    return total + loop_sum;
}
