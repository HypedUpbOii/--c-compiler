// Function calls inside nested if-else

int add(int x, int y) {
    int z;
    z = x + y;
    return z;
}

int sub(int x, int y) {
    int z;
    z = x - y;
    return z;
}

void main() {
    int a, b, c;

    a = 10;
    b = 5;

    if(a > b) {
        if(b > 0) {
            c = add(a, b);
        } else {
            c = sub(a, b);
        }
    } else {
        c = sub(b, a);
    }

    print c;
}