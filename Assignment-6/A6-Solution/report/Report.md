
---
Points to talk about

- What we fixed
1) We fixed array indexing
2) We use correct pointer types (4 bytes instead of 8 bytes for float pointers)
3) We fixed how globally defined arrays work

- What we added
1) We made a few syntax changes like adding a for loop and creating break and continue statements
2) We added unreachable code elimination in the TAC stage
3) We allow pointers and arrays to be passed around as parameters in functions.

Add examples here
```cpp
#include <iostream>
int main
```

```mips
li $t1, 1
lw $v1, -4($fp)
```

```tac
temp1 = temp0 + 
```