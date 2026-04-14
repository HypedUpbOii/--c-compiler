// illegal read statements

bool f(string a) {
    bool x;

    return x;
}

void main() {
    int b;

    bool x;

    string a;

    read a;

    x = f(a);
}