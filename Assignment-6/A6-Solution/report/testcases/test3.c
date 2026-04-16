int main() {
    float x;
    float *y;
    float **z;

    x = 3.502;

    print "Initial value of x :";
    print x;
    print "\n";
    
    y = & x;
    z = & y;

    **z = 5.2034;

    print "Final value of x :";
    print x;
    print "\n";

    return 0;
}