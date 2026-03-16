

int accumulator = 0;

int square(int value) {
    return value * value;
}

int factorial(int n) {
    int result = 1;
    int i;
    for (i = 1; i <= n; i++) {
        result *= i;
    }
    return result;
}

int clamp(int number, int minimum, int maximum) {
    int output = number;
    if (output < minimum) {
        output = minimum;
    } else if (output > maximum) {
        output = maximum;
    }
    return output;
}

int main() {
    int i = 0;
    while (i < 5) {
        accumulator += i;
        i++;
    }

    int fact = factorial(4);
    int sq = square(accumulator);
    int limited = clamp(fact, 5, 1000);

    if (limited > sq) {
        accumulator = limited;
    } else {
        accumulator = sq;
    }

    accumulator += fact;
    accumulator -= sq;

    return accumulator;
}
