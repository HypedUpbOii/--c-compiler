// loop : nested while, do-while statement with combination of operators in condition

void main(){

    int p,q,r,i,t;
    read p;
    read q;
    read r;
    read i;
    do{
        p=p-100*i;
        q=q-150*i;
        r=r-50*i;

        while(i>10 && i<=20) {
            i=i+1;
        }

    }while(((p>q ? (p>r ? p : r) : (q>r ? q : r)) > 0) && (i>10 && i<=20));

    print ((p>q ? (p>r ? p : r) : (q>r ? q : r)));
}
