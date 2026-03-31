// Nested ternary operator with logical expressions
int global_x;

void main(){
    int ans;

    read global_x;

    ans = (global_x > 0) ? 1 : ((global_x == 0) ? 0 : -1);

    print ans;
}