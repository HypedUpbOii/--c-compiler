// Testing whether combination of arithmetic, logical, ternary and relational operators are handled correctly
void main()
{
    string res;
    string code;
    int a,b;
    code = (a+5==b-3? a*1<b/4 : a-3>b*2)? res : "nope";
    print code;
}