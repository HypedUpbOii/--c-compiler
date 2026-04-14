// Tail recursion with void return

void tail(int n) {
    if(n != 0){
        print(n);
        tail(n - 1);
    }
}

void main()
{   
    int a;
    a = 5;
    tail(a);
}