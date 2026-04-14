// loop : non-nested while, do-while statement with simple boolean expression

void main()
{
   int a,b,c,d;
   bool bl;
   read a;
   read b;
   read c;
   read d;
   bl = a>b;
   while(bl) {  
      read a;
      read b;
      bl = a>b;
    }
    read c;
    do{
        print a;
        print b;
        a=a-c;
        b=b+c;
        read c;
    }while(c>0);   
}

