// Non boolean expression inside if condition
void main() {
    int a, b, c;
    a = 1;
    b = 2;
    c = 3;
    if ((a + b) * c) {
        print c;
    }
    else {
        print a;
    }
}