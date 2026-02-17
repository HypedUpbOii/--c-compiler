
void main()
{
    int num1,num2,greater;
    string a, b;
    float num3;
    num1 = -10000000000000000;
    read num1;
    read num2;
    greater = num1>=num2 || num1*num2 > num1+num2 ? num1 : num2;    
    print greater;
}
