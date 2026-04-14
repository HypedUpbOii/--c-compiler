// loop : non-nested while, do-while statement with combination of operators in condition

void main()
{
   int a,b,c,d,e;
   bool bl;
   read a;
   read b;
   read c;
   read d;
   read e;
   while((a+c > b+d ? c : d) > e ) {  
      read c;
      read d;
    }
    print a+c;
    print b+d;
    do{
        print a+b+c+d+e;
        e=e-1;
        read a;
    }while(e>0 || a<0);
}    
