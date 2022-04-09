#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "data.h"


int upper(int a) {
  a -= 32 * (a >= 'a' && a <= 'z');
  return a;
}//smaller

int main() {
  int i = 0;
  clock_t start = clock();
  for (i=0; i < 1000000; i++) {
    printf("%d\n", upper(data_a[i]) );
  }

  clock_t end = clock();

  double time_spent = (double)(end - start) / CLOCKS_PER_SEC;

  printf("\ntime taken = %f\n", time_spent);

  return 0;

}//main
