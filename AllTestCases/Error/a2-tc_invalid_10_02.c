// Tests type mismatch in operands of ternary operator
void main()
{
    int a,b,c;
    float d,e;
    a = b==c?d:e;
    print a;
}