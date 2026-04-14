// Simple ternary operator with logical expressions
int global_a;
int global_b;

void main() {
    int ans;

    read global_a;
    read global_b;

    ans = ((global_a != global_b) && (global_a > global_b) ? 1 : 0);

    print ans;
}