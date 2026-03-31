// Bool and string variables can't be compared with relational operators


void main() {
    string str,str2;
    bool b;
    bool res;

    str = "Okay";
    str2 = "hello";
    b = 4 < 1;
    res = 5 == 4;
    res = b > res;
}