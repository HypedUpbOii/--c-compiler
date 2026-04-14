// Function call inside do-while loop

int square(int x) {
    int y;
    y = x * x;
    return y;
}


void main() {
    int a, i;

    read a;
    i = 0;

    do {
        a = square(a);
        i = i+1;
    } while(i < 10);

    print a;
}