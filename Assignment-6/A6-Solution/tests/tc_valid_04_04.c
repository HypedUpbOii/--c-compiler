// Functions as operands in arithmetic expressions with braces
int add(int a, int b);
int add(int a, int b) {
    return a + b;
}

void main() {
    int result, t1, t2;
    t1 = add(2,3);
    t2 = add(4,5);
    result = (t1 + t2);
}