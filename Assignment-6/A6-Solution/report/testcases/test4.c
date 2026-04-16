int main() {
    int a, b;
    int *x, *y;

    a = 7;

    x = & a;
    y = x;

    print y;

    return 0;
}