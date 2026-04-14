// Function calls with return type mismatch

int globe;
bool b_globe;

void func_def(int x, bool y, string z){
    x = globe;
    y = b_globe;
    z = "why?";
}

void main(){
    int a;
    bool b;
    string s;
    read globe;
    read a;
    b_globe = globe > 10;
    a = func_def(a,b,s);
    print a;
}