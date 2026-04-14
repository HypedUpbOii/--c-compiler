// Self recursion : function call with parameter mismatch

int f(int i){
    if(i<=0){
        return 0;
    }
    i=i-1;
    return f(i, i+1);
}

void main(){
    int i;
    read i;
    print f(i);
}