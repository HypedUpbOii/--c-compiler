// Checks arithematic expressions without braces

int p, q, r;

void main(){
    int a, b, c, d;

    p = -2;
    q = 14;
    r = 37;

    a = p - -q * r;
    b = - -p/ q + r;
    c = - -c  - r + a;

    p = d * -a + b;
    q = - -c * r + a - b;
    r = c * 800 / 41;

    print p;
    print q;
    print r;
}