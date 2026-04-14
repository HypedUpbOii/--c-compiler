// Tests combination of relational, logical and ternary operators
void main()
{
    int a,b,c;
    bool d;
    d = a<b && b<c || a==c? a<b : b>=c;
}