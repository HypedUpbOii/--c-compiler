// Assignment statement where rhs is constant

int func_def(int x, bool y, string z){
    x = 10;
    y = x > 2*x;
    z = "why?";
    return x;
}

void main(){
    int a;
    bool b;
    string s;
    read a;
    a = func_def(a,b,s);
    print a;
}