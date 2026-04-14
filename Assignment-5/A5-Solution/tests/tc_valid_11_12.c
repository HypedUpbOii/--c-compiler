// Combination of different local and global variable declarations

int my_global;
bool my_local;

void func_def(){
    int my_local;
    my_local = my_global;
    my_global = my_global + 10;
}

void main(){
    read my_global;
    my_local = my_global > 1;
    func_def();
    print my_global;
}