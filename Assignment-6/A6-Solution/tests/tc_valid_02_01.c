// Function call inside while loop

int inc(int x) {
    int y;
    y = x + 1;
    return y;
}


void main() {
    int a, i;

    read a;
    i = 0;

    while(i < 5) {
        a = inc(a);
        i = i+1;
    }

    print a;
}