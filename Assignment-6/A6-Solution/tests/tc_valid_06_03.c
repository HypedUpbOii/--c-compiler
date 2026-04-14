// nested ternary operator

int check1() {
    return 5;
}
int check2() {
    return 10;                                                     
}

void main() {
    int a, c1, c2;
    c1 = check1();
    c2 = check2();
    a = c1 > 10 ? c2 : c2 < 10 ? c2 - 5 : 3;
}