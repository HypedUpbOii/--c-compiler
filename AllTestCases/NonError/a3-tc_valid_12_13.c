// Checks for combination of arithmetic, logical, relational and ternary operators

void main(){
    int a, b, c, d, result;
    a = 5;
    b = 7;
    c = 10;
    d = 8;

    result = ((a * 2 + b / 3 > c - d) && (b -- a < c)) ? ((a + b * c) / (d + 1) > 10 ? a * b + c : b - c * d) : ((c + d > a) || ( a * b < 50) ? a + c * d : b / (a + 1));

    print result;
}