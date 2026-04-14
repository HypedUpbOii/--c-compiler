// simple ternary operator

int check() {
    return 1;
}                                                                                   

void main() {
    int a, b;
    a = 10;
    b = check();
    a = a < 10 ? b : 0;
}