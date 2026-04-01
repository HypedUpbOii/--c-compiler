void main(bool b, int a);
int c;
void main(int a, bool b){
    int num1, num2, greater;
    read num1;
    read num2;
    greater = num1 >= num2 || num1 * num2 > num1 + num2 ? num1 : num2;
    print greater;
}