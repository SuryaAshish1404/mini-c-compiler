int main() {
    int sum = 0;
    int i = 0;

    while (i < 5) {
        int j = 0;
        while (j < 4) {
            if ((i + j) % 2 == 0) {
                sum = sum + i + j;
            } else {
                sum = sum - i;
            }
            j = j + 1;
        }
        if (sum > 10) {
            sum = sum - 3;
        } else {
            sum = sum + 2;
        }
        i = i + 1;
    }

    return sum;
}
