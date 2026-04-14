// Nested if-else with boolean variable/relational operator

void main() {
    int val;
    bool b;
    read val;
    b = val < 0;

    if (val > 5) {
        if (val > 6 && val < 7) {
            if (val == 8 || val == 0 || b) {
                print "wow";
            } else {
                print "not wow";
            }
        }
    }
}