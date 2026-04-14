// Check logical operations with non-booleans (assigning bool to int directly)

void main() {
    int b;
    bool d;
    b = 3;
    d = 1 == 1;
    b = d;
}