




int globalCount;
float pi = 3.14;


float mix(float lhs, float rhs) {
    float total = lhs + rhs;
    return total / 2.0;
}

void spin_counter() {
    int loops = 3;
    while (loops > 0) {
        loops--;
    }
    return;
}


int add(int leftOperand, int rightOperand) {
    return leftOperand + rightOperand;
}

int max_pair(int leftValue, int rightValue) {
    int bigger;
    if (leftValue > rightValue) {
        bigger = leftValue;
    } else {
        bigger = rightValue;
    }
    return bigger;
}


int main() {
    
    int x = 10;
    int y = 20;
    int result;

    
    result = x + y * 2;

    
    if (result > 50) {
        result = result - 10;
    } else {
        result = result + 10;
    }

    
    int counter = 0;
    while (counter < 5) {
        counter++;
    }

    
    {
        int shadow = 5;
        result += shadow;
    }

    
    int sum = 0;
    for (int i = 0; i < 10; i++) {
        sum += i;
    }

    
    if (x == 10 && y == 20) {
        if (sum > 0 || counter != 0) {
            result = sum + counter;
        }
    }

    
    int total = add(x, y);
    int bigger = max_pair(total, result);
    float midpoint = mix(pi, 2.5);
    spin_counter();

    
    int neg = -x;
    int flag = !0;

    
    total += 100;
    total -= 50;
    total *= 2;
    total /= 3;

    int midpointRounded = midpoint;
    bigger += midpointRounded;

    return 0;
}
