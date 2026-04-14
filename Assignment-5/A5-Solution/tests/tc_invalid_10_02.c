// Self Recursion : function calls with return type mismatch

float a,b;

float f()
{   
   float c; 
   if(a+b<0){
    return "0.0";
   }
   a=a-5;
   b=b-10;
   c = f();
   return a+b+c;

}
void main()
{
    read a,b;
    print f();
}
