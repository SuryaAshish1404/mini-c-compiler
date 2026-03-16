int choose(int x) {
    int value = -1;
    switch (x) {
        case 0:
            value = 10;
            break;
        case 1:
            value = 20;
            break;
        default:
            value = 30;
            break;
    }
    return value;
}

int main() {
    int total = 0;
    for (int i = 0; i < 3; i++) {
        total += choose(i);
    }
    return total;
}
