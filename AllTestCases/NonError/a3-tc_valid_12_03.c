// Testing whether combination of arithmetic, logical, ternary and relational operators are handled correctly
float g1, g2;

void main()
{
    // double nesting of first operand of ternary
    float f;
    f = ((-g1<=3.0*g2? 4.0+g2>3.0+g2 : g1+g2<g2)? g1/g1 >=g2-g1 : g1==g2)? g1+g2 : g2/g1;
}