// combination of use of local and global variables
int a;
float b;
bool c;
string d;

int main()
{
    int la;
    float lb;
    bool lc;
    string ld;
    int result1;
    float result2;
    bool result3;
    string result4;

    result1 = a - la;
    result2 = b / lb;
    result3 = c || lc;
    result4 = result3? d : ld;

    return 0;
}