#include <stdio.h>
#include <stdlib.h>

#include "data.h"


int smaller(int a, int b) {
  if (a < b)
    return a;
  else
    return b;
}//smaller

int main() {
  int i = 0;
  for (i=0; i < 1000000; i++) {
    printf("%d\n", smaller(data_a[i], data_b[i]) );
  }

}//main
