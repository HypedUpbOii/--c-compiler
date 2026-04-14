// Checks for combination of arithmetic, logical, relational and ternary operators
void main(){
    int a, b, c, d, e, f;
    float x, y, z, w;
    bool p,q,r,s,t;

    a = (q ? d : e) / ((!r && s || t) ? (a --b * c / (d + f)) : f) / ((!(t && q) || (p || s)) ? (--f - d + (a + b) / (c + e)) : (b));
}