// avoid function calls without a return
// and parameters
void compute(int x) {
    x = x+1;
    print x;
}

void main() {
    compute(5);
}