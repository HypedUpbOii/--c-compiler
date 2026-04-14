// Non boolean expression inside while condition
void main() {
    int a, b, c;
    a = 2;
    b = 3;
    c = 4;
    while ((a + b) * (c - a)) {
        a = a + 1;
    }
}