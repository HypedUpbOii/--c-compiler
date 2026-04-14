// ternary operator + relation operator

int f() {
    return 5;
}

void main() {
    int a, b;
    b = f();
    a = (b + 10) > 13 ? b : b + 3;
}