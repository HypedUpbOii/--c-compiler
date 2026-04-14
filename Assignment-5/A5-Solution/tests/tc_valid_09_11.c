// if-else with multiple returns

int f(int a, int b) {
    if(a > 5) {
        return a;
    } else if (a > 5 && b < 5) {
        return a + b;
    } else {
        return b;
    }
}

void main() {

    int a, b, c;

    c = f(a, b);

    print c;
}