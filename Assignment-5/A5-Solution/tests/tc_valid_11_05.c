// Assignment statements where rhs is a variable/expression
int a;
float b;
bool c;
string d;

int main()
{
    int al;
    float bl;
    bool cl;
    string dl;

    int r1, r2;
    float f1, f2;
    string s1, s2;

    a = r1/r2-r1*r2+r2;
    al = a+r2/r1*(r2-r1);
    b = f1/f2/f1/f2+f2;
    bl = b;
    c = r1>r2 || f1!=f2;
    cl = c && f2==f1;
    d = c? s1 : s2;
    dl = cl? s2 : s1;

    return 0;
}