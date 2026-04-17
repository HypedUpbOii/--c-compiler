int arr[100][100];

int main() {
    int i;
    int j;

    for (i = 0; i < 100; i = i + 1;) {
        for (j = 0; j < 100; j = j + 1;) {
            arr[i][j] = i - j + (i * j);
        }
    }

    for (i = 0; i < 10; i = i + 1;) {
        print "arr[";
        print i;
        print "]";
        print "[";
        print i;
        print "] = ";
        print arr[i][i];
        print "\n";
    }

    return 0;
}