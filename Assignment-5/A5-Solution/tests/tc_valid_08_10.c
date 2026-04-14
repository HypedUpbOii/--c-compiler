//Function with conditional printing
int x;

void check(){
    if (x>0)
        print "Positive";
    else if (x==0)
        print "Zero";
    else
        print "Negative";

}
void main(){
    read x;
    check();    
}