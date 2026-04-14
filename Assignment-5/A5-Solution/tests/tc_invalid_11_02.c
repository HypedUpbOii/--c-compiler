// illegal read statements

bool f() {
    bool x;

    read x;

    return x;
}

void main() {
    int a, b;

    bool x;

    x = f();
}