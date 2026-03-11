void main(int a);

void main(int a){
    int num1, num2, greater;
    read num1;
    read num2;
    greater = num1 >= num2 || num1 * num2 > num1 + num2 ? num1 : num2;
    print greater;
}