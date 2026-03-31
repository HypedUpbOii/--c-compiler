// Data type mismatch with ternary operator

void main() {
    int a, b, c, d;

    a = (b > c) ? (c < d && a < b) : (d > a || b < c);
}