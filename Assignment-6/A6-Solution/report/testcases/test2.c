int main() {
    int x;
    int *y;
    int **z;

    x = 3;

    print "Initial value of x :";
    print x;
    print "\n";
    
    y = & x;
    z = & y;

    **z = 5;

    print "Final value of x :";
    print x;
    print "\n";

    return 0;
}