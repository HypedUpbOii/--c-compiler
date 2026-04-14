//Multiple function calls with print and read statements
int a;
float b;

void readValues(){
    read a;
    read b;
}

void printValues(){
    print "a:";
    print a;
    print "b:";
    print b;
}

void main(){
    
    readValues();
    printValues();
}