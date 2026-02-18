float a;

void main(float b){
    bool x;
    a = (a > b) ? a : ((a == b || b == a && b == a) ? b : a);
    x = !x || !x && !!!x;
}