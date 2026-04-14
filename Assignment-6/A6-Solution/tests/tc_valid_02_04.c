// Combination of function call inside while loop and nested if-else

int inc(int x) {
    int y;
    y = x+1;
    return y;
}

int dec(int x) {
    int y;
    y = x-1;
    return y;
}

void main() {
    int a, i;

    read a;
    i = 0;

    while(i < 5) {
        if(i < 2) {
            a = inc(a);
        } else {
            if(i < 4) {
                a = dec(a);
            } else {
                a = inc(a);
            }
        }

        i = i+1;
    }

    print a;
}