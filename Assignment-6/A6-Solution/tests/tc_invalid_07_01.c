// Function calls with return type mismatch
int a;
int b;
int c;

int globe;
int country;

void is_country(){
    int country;
    country = globe - 1;
    return country;
}

void main(){
    int state;
    country = 2;
    globe = 5;
    state = is_country();
    print state;
}
