int gcd(int a, int b) {
    while (b != 0) {
        int temp = a % b;
        a = b;
        b = temp;
    }
    return a;
}

int is_prime(int num) {
    if (num < 2) {
        return 0;
    }

    int i = 2;
    while (i * i <= num) {
        if (num % i == 0) {
            return 0;
        }
        i++;
    }

    return 1;
}

int fibonacci(int n) {
    if (n <= 1) {
        return n;
    }

    int a = 0;
    int b = 1;
    int i = 2;

    while (i <= n) {
        int temp = a + b;
        a = b;
        b = temp;
        i++;
    }

    return b;
}

int main() {
    int x = 10;
    int y = 15;
    int z;

    if (x > y) {
        z = x - y;
    } else if (x < y) {
        z = y - x;
    } else {
        z = 0;
    }

    int result = gcd(x, y);

    int count = 0;
    for (int i = 1; i <= 20; i++) {
        if (is_prime(i)) {
            count++;
        }
    }

    int fib = fibonacci(10);

    return result + count + fib + z;
}
