int main() {
    int x = 5;
    int y = 10;
    int z;

    if (x > 3) {
        z = x + y;
    } else {
        z = x - y;
    }

    int result = z;

    int sum = 0;
    int i = 0;
    while (i < 5) {
        sum = sum + i;
        i = i + 1;
    }

    return result + sum;
}
