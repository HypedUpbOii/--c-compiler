// Checks return statement in ternary expressions

string isEven(int a){
    return ((a/2)*2 == a ? "Even" : "Odd");
}

void main(){
    int a;
    string ans;
    read a;
    ans = isEven(a);
}
