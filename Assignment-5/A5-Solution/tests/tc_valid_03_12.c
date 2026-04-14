// tail recursion
int g;

int fact(int n, int acc);

int fact(int n, int acc) {
    int temp;
    if (n <= 0){
        return acc;
    }
    else{
        temp = fact(n-1, acc*n);
        return temp;
    }
}

void main() {
    int result;

    g = 1;
    result = fact(4, 1);

    print result;
}