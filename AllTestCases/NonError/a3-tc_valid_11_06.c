// Nested ternary operator with logical expressions
int g1;
int g2;

void main(){
    int ans;

    read g1;
    read g2;

    ans = ((g1 > 0) && (g2 > 0)) ? 1 : ((g1 == 0 || g2 == 0) ? 0 : -1);

    print ans;
}