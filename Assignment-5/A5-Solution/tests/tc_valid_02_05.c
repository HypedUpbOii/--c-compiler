// Function call inside do-while loop and nested if-else statements

int add(int a, int b) {
    int c;
    c = a+b;
    return c;
}

int sub(int x, int y) {
    int z;
    z = x - y;
    return z;
}


void main() {
    int a, b, i;

    a = 5;
    b = 3;
    i = 0;

    do {
        if(i < 2) {
            a = add(a, b);
        } else {
            if(a > b) {
                a = sub(a, b);
            } else {
                a = add(b, a);
            }
        }

        i = i+1;
    } while(i < 4);

    print a;
    print b;

}