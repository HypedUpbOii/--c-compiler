// Checks for invalid combination of arithmetic, logical, relational and ternary operators

void main(){
    int x;
    float y, p;
    bool z, w;

    p = ((x * y < 40) || (z > w)) ? ((x + z * 3) ? (x && y) : (z + w)) : ((x < y && z) ? (y / w) : ( x- y));
}