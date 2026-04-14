// Functions as operands in arithmetic expressions with braces
int sum(int a, int b, int c) {
    return a + b + c;
}

int max(int a, int b, int c) {
    if (a > b) {
        if (a > c) {
            return a;
        }
        else {
            return c;
        }
    }
    else {
        if (b > c) {
            return b;
        }
        else {
            return c;
        }
    }
}

int min(int a, int b) {
    if (a < b) {
        return a;
    }
    else {
        return b;
    }
}

int avg(int a, int b) {
    return (a + b) / 2;
}

void main() {
    int t1, t2, t3, t4, total;
    t1 = sum(1,2,3);
    t2 = max(4,5,6);
    t3 = min(7,8);
    t4 = avg(9,10);
    total = (t1 + t2) * (t3 - t4);
}