// using function in print stmt
int square(int f);

int square(int f){
    int ans;
    ans = f * f;
    return ans;
}

int  main(){
    int s;
    int ans;
    print square(5);
    // ans = square(s);
    // print ans;
    return 0;
}