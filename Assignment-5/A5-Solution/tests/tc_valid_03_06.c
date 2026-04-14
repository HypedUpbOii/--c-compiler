// Non Head and Non Tail recursion with non void return
int rec(int n);

int x, y;
int rec(int n) {
    if(n == 0) return x;
    x = x + 1;
    y = rec(n - 1);
    return 2 * x + y;
}

void main()
{   
    int a;
    a = 5;
    a = rec(a);
}