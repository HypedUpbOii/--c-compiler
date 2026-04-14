// nested ternary operator

int check() {
    return 5;
}

void main() {
    int a, b;
    b = check();
    a = b > 10 ? 1 : b > 2 ? 1 : 3;
}