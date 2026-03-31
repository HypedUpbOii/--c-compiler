// Simple ternary operator with logical expressions
int global_min;
int global_max;

void main(){
    int x;
    int ans;
    global_min=10;
    global_max=100;

    read x;

    ans = ((x >= global_min) && (x <= global_max) ? 1 : 0);

    print ans;
}