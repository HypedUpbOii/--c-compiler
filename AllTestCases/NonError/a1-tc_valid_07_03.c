// Checks arithematic expressions without braces

int a, b, c;
void main(){
    int d, e;
    read a;
    read b;
    read c;

    d = 4;
    e = 2;

    d = - -a - -b * c + a;
    e = d + -a / b - c;

    a = - -e * b - -d + c;
    b = a - -b + -c * d;

    c = - a - -b - - c + 1;
    print a + b;
}