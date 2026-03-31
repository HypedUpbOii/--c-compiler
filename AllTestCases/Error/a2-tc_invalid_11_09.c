// Comparison of comparisons is illegal

void main() {
    int a, b, c, d;

    a = ((b > c) == (c > d)) > (a < b);
}