// ternary operator + relation operator + logical operator

int f1() {
    return 5;
}
int f2() {
    return 10;
}

void main() {
    int a, b, c;
    b = f1();
    c = f2();
    a = (b > 2 && c < 12) ? b + c : b;
}