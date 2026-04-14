// Checks for invalid combination of arithmetic, logical, relational and ternary operators

void main(){
    int a, z;
    float b;
    bool c, d;

    a = 5;
    b = 3.2;
    z = ((a + b * 2 > 20) && (c + d)) ? ((a > c) ? a + (b && a) : (c < d? a : b)) : ((a || b) ? a * c : d + b);
}