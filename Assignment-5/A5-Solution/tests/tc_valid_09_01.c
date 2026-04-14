//Checks return statements in if-else statements

string grade(int a){
    if(a >= 90) return "A";
    else if(a >= 80) return "B";
    else if(a >= 70) return "C";
    else return "Fail";
}

void main(){
    int a;
    string b;
    b = grade(a);
    print(b);    
}
