// Head recursion with non void return
int res;
int head(int n) {
    if(n == 0)
        return 0;
    res = head(n - 1);
    print(n);
    return res + n;
}

void main()
{   
    int a;
    a = 5;
    a = head(a);
}