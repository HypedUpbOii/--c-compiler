// Checks for combination of arithmetic, logical, relational and ternary operators

void main(){
    int x, y, z, w, value;
    x = 2;
    y = 4;
    z = 10;
    w = 5;

    value = ((x + y * z <= 50) || (z - x > y && w * 3 < x)) ? ((x + y == 0) ? (x * z + y / w > 60 ? x + z : y * w + z) : (z / w + x * y)) : ((x -  y * w > 5 && z - x!= 0) ? (x * y - z) : (z + w * y));
}