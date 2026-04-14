// Define a variable with same name in global and local scope.
int globe;
int country;

int is_country(){
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