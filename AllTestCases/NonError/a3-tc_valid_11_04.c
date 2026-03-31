// Simple ternary operator with logical expressions
int global_input;

void main(){
    int ans;

    read global_input;

    ans = ((global_input / 3 == 0) || (global_input / 5 == 0) ? 1 : 0);

    print ans;
}