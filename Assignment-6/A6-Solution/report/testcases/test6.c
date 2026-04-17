void foo(int a[][20]) {
    a[1][15] = 21;
    a[0][14] = 2;
}

void bar(int b[][20]) {
    b[0][13] = 44;
    foo(b);
}

void main() {
    int a;
    int arr[10][20];
    
    arr[1][15] = 7;

    print "arr[1][15]: ";
    print arr[1][15];
    print "\n";

    bar(arr);

    print "arr[1][15]: ";
    print arr[1][15];
    print "\n";

    print "arr[0][14]: ";
    print arr[0][14];
    print "\n";

    print "arr[0][13]: ";
    print arr[0][13];
    print "\n";
}
