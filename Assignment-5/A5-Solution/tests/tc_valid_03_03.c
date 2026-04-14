// Non Head and Non Tail recursion with void return

void rec(int n) {
    if(n != 0) {
        print(n);
        rec(n - 1);
        print(n);
    }
}

void main()
{   
    int a;
    a = 5;
    rec(a);
}