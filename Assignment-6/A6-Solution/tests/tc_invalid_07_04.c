// Function call with parameter mismatch
int globe;
bool b_globe;

int func_def(int x, bool y, string z){
    x = globe;
    y = b_globe;
    z = "why?";
    return x;
}

void main(){
    int a;
    bool b;
    string s;
    read globe;
    read a;
    b_globe = globe > 10;
    a = func_def(a,globe,s);
    print a;
}