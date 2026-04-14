// Type mismatch in ternary operator expressions
float g1;
float g2;

void main()
{
    // double nesting in 1st, 2nd, and 3rd arguments of ternary operator
    float f;
    f = ((-g1<=3.0*g2? 4.0+g2>3.0+g2 : g1+g2<g2)? g1/g1*g2-g1 : g2)? (g1+g2 >= g1-g2? g1/g2 : g2/g1) : (3==4? g2 : g1*1.50);
}