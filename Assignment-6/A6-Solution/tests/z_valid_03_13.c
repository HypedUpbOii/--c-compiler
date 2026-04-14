// neither head nor tail recursion

int a;

void h(int n){
    if (n <= 0){
        
    }
    else{
        a = a+1;
        h(n-1);
        print a;
    }
    
}

void main(){
    a = 0;
    h(3);
}