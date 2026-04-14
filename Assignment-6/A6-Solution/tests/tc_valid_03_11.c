// head recursion

int x;

int f(int n) {
    int dummy;
    if(n <= 0){
        return 0;
    }

    dummy = f(n-1);
    x = x+n;

    return x;
}

void main() {
    int res;
    x = 0;

    res = f(3);
    print res;
}