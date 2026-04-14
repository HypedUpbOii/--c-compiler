// Checks return statement in loops

string f(int a){
    while(a <= 200){
        a = a+10;
        if((a/11)*11 == a) return "Number divisible by 11";
    }  
    return "Number not found";
}

void main(){
    int num;
    string output;
    output = f(num);
}
