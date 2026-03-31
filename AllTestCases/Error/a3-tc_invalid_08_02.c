// Invalid operands to logical operators

void main() {
    int a;
    bool b;
    a = 123123;
    b = b && b;
    b = b || b || b;
    b = b && b || !b;
    b = b && b || a
}