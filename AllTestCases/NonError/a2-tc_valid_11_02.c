// Checks nested ternary operator expressions

void main(){
    int a, b, c;
    float  x, y, z;
    bool p, q, r;

    z = (p || q && p) ? (x + y) : (z - y);
    c = (q && (p || (p && q))) ? ( (p && q) ? (a + b / c - 80 / a) : (p || (r && q)) ? (b + 40/45) : 30) : (--b / a / a);
}