//Checks nesting of parentheses with long chain of unary minus operators

int main(){
    
    int main, a, b, c, x, y;

    read b;
    read c;
    read x;
    read y;

    a = ((-------c ------b + ------- y + ------- x) * x / c) / (y * c / -x + b * (c - b + y));
    main = ( ((((((-a)))))) + ------b);
    
    print (main);

}