void swap(int * a, int * b) {
    int temp;
    temp = *a;
    *a = *b;
    *b = temp;
}

void main() {
    int a, b;
    int *p, *q;

    p = &a;
    q = &b;

    a = 5;
    b = 10;

    print "Initial values a = ";
    print a;
    print " , b = ";
    print b;
    print "\n";

    swap(p, q);

    print "Final values a = ";
    print a;
    print " , b = ";
    print b;
    print "\n";

}
