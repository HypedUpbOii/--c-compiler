// Head recursion with void return
void hello(int hahaha);

void head(int n) {
    if(n != 0) {
        head(n - 1);
        print(n);
    }
}

void main()
{   
    int a;
    a = 5;
    head(a);
}