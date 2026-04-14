// ternary operator + relation operator + logical operator

int f() {
    return 5;
}

void main() {
    int a, b;
    b = f();
    a = (b > 2 && b < 5) ? b : (b < 10) ? b - 5 : b - 10;
}