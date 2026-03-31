// type mismatch in ternary operator condition
int global1;

void main(){
    int x;
    float ans;

    read x;

    global1 = x + 5;

    ans = (global1 > 10.01) ? x : -global1;

    printt ans;
}