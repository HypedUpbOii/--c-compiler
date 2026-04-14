// Nested ternary operator with logical expressions
int value;

void main(){
    int ans;

    read value;

    ans = (value < 10) ? 1 : ((value < 50) ? 2 : 3);

    print ans;
}