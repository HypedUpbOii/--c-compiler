int foo();

int add(int a, int b) {
    return (a > b) ? (a + 2 * b) : (a * 3 + b);
}

int foo() {
    return 5;
}

int main() {
    int a, b, c;
    c = add(a, b);
    print c;
    return 0;
}