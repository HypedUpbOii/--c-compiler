// Checks arithematic expressions without braces

void main(){
    int a, b, c, d;

    read a;
    b = 0;

    c = - -a + b;
    d = a ----b * a;
    a = ---c - b + a;
    b = a * --b - c;

    print a;
    print b;
    print c;
    print d;
}