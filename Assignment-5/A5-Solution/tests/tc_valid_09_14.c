// Combination

int g(int a) {
    int b;
    return b>5? b:5;
}

string f(int x, int y) {
    int i, j;
    i = x;
    while(i>0) {
        j = g(i);
        if(j > y) {
            return i>2?"POSITIVE UP":"POSITIVE DOWN";
        } else if (j == 0) {
            return i>0?"ZERO":"";
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