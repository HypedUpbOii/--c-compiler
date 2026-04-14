// Non-void function with parameters

int mul(int m, int n);

int mul(int m, int n){
    int multiplication;

    multiplication = m * n;

    return multiplication;
}

void main(int z){
    int multiplication;

    multiplication = mul(z,45);
    print multiplication;
}