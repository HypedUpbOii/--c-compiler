// If-else ladder from function return value.

int c_to_f(int c) {
    return (c / 5) * 9 + 32;
}

void main() {
    int val;
    read val;
    val = c_to_f(val);
    if (val > 102) {
        print "bad fever";
    } else if (val > 100) {
        print "fever";
    } else if (val > 98) {
        print "maybe fever";
    } else {
        print "no fever";
    }
}