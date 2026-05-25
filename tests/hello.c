#include <stdio.h>

.macro hello
int main() {
  printf("hello, world\n");
  return 0;
}
.endm
hello
