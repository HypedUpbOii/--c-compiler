// Checks for function calls with control structure

int g(){
    int x;
    x = 1 + 2 / 3;
    return x;
}
void main(){
    int a,b,c;
    if(a == b){
        c = g();
    }
    else{
        print c;
    }
}