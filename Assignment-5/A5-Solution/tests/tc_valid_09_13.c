// loops with returns

int g(int a) {
    int b;
    return b;
}

string f(int x, int y) {
    int i, j;
    i = x;
    while(i>0) {
        j = g(i);
        if(j > y) {
            return "POSITIVE";
        } else if (j == 0) {
            return "ZERO";
        } else {
            return "NEGATIVE";
        }
        i = i+1;
    }
}

void main() {
    int a, b;

    string s;
    s = f(a, b);

    print s;
}