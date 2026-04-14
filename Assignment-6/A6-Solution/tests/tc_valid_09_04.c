// Checks return statement in loops with ternary and if expressions

int f(int n){
    int i, m;
    string result;
    i = 0;
    while(i < n){
        read m;
        if(m < 0) return 0;
        result = (m < 50) ? "small" : "large";
        print(m);
        print(result);
        i = i+1;
    }
    return 1;
}
int main(){
    int ans;
    ans = f(12);
    return 1;
}