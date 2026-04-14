// Assignment operator instead of comparison

void main() {
    int a, b, c;

    a = ((b = c) > 0) ? b : c;
}