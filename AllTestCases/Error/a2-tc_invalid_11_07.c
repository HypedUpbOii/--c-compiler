// Data type mismatch with complex combination of operators on RHS

void main() {
    int a, b, c, d, e;

    a = ((b + (c > d ? e : d)) > (c + d)) && (a < b);
}