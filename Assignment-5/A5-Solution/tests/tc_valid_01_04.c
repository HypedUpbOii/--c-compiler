// Multiple function (void + non-void) with main

void printCube(int c);

int cube(int p);

void printCube(int C){
    print C;
}

int cube(int P){
    int ans;
    ans = P * P * P;

    return ans;
}

void main(){
    int p;
    int ans;
    p = 76;
    ans = cube(p);
    printCube(ans);
}