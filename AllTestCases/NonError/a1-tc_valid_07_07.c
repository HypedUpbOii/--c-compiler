// Checks arithematic expressions without braces

void main(){
    int a, b, c, d, e, f;
    
    a = 20;
    b = 5;
    c = 3;
    d = 2;

    e = a + b * c - d;
    f = a - b - c + d;

    a = e * b + f / c - d;
    b = a / b + c * d - e;

    c = a - b * c + d + 10;
    d = c / b * a - f + 3;

    print a;
    print b;
    print c;
    print d;
}