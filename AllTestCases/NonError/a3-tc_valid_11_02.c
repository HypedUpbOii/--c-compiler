// Simple ternary operator with logical expressions
bool global_flag;

void main(){
    int n;
    int result;

    read n;

    global_flag = (n > 0);

    result = (global_flag && (n / 2 ==0) ? 1 : 0);

    print result;
}