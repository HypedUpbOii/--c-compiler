// Tail recursion with non void return

int tail(int n, int s) {
    int z;
    if(n == 0) 
        return s;
    z = tail(n - 1, s + n);
    return z;
}

void main()
{   
    int a;
    a = tail(3, 100);
}