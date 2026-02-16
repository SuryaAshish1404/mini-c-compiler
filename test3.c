/* ============================================================
 * test3.c — Valid program highlighting nested scopes & returns
 * ============================================================ */

int globalFlag = 1;

float blend(float x, float y, float alpha) {
    float mix = (x * alpha) + (y * (1.0 - alpha));
    return mix;
}

int choose(int lhs, int rhs, int useFirst) {
    if (useFirst) {
        return lhs;
    }
    return rhs;
}

void bump_counter(int* valuePtr) {
    // pointers unsupported in grammar, so simulate with global
}

int compute_series(int limit) {
    int total = 0;
    int k;
    for (k = 0; k <= limit; k++) {
        total += k;
    }
    return total;
}

int main() {
    int base = 5;
    int bonus = 3;
    int toggle = 1;

    int picked = choose(base, bonus, toggle);
    int accumulation = compute_series(6);
    float average = blend(picked, accumulation, 0.25);

    if (average > 10.0) {
        int offset = 2;
        accumulation = accumulation + offset;
    } else {
        accumulation = accumulation - picked;
    }

    while (accumulation > 0) {
        accumulation -= 4;
    }

    if (globalFlag) {
        accumulation += picked;
    }

    return accumulation;
}
