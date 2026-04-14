// Functions as operands in arithmetic expressions without braces
int compute(int a, int b) {
    return a * b + a;
}

int finalize() {
    return 10;
}

void main() {
    int a, b, t1, t2, result;
    a = 2;
    b = 3;
    t1 = compute(a, b);
    t2 = finalize();
    result = t1 + t2;
}