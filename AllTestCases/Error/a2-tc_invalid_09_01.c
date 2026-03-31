// invalid relational operator expressions : bool variables cannot be compared 

bool b1;

void main(){
    bool b2;
    int x,y;
    b1 = x>y;
    b2 = (x+y) > (--x);
    b2 = b2 >= b1;
}