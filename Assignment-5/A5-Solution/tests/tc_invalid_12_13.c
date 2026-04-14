// type mismatch in ternary operands
void main() {
    int a;
    bool cond;
    string s;

    a = 5;
    cond = 2 > 1;

    a = cond ? a : s;
}