// Tests whether combination of arithmetic and ternary operators are handled correctly
void main()
{
    int a, b, c;
    read a;
    read b;
    c = a*b >= b/a+c? a/b : -c;
    print c;
}