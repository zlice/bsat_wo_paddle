#include <stdio.h>
#include <stdlib.h>
#include "lookup_table.h"
#include <time.h>
#include <string.h>

// memory stats from /proc/<pid>/status
static inline int mem_used(void)
{
   char  status[256];
   pid_t pid = getpid();

   sprintf(status, "/proc/%d/status", pid);
   FILE* in = fopen(status, "rb");
   if (in == NULL) return 0;

   // grep VmPeak
   int peak_kb = 0;
   while (!feof(in) && fscanf(in, "VmPeak: %d kB", &peak_kb) != 1)
       while (!feof(in) && fgetc(in) != '\n')
           ;
   fclose(in);

   printf("mem used = %f MB\n", peak_kb/(double)1024);
   //return peak_kb;
}

int main() {

   register int i = 0;
   // could reorganize and take higher input, but this should be fine
   unsigned long int pos[125] = {0};
   unsigned long int neg[125] = {0};
   register unsigned char cur = 0;

   char bsat[5000];
   printf("gimme bsat: ");
   gets(bsat);
   printf("sizeof bsat = %d\n", strlen(bsat) );

   clock_t begin = clock();

   for (i=0; i < strlen(bsat); i++) {
      cur = (unsigned char)bsat[i];
      switch(cur) {
        case '!':
          cur = (unsigned char)bsat[++i];
          neg[dec(cur)] = 1;
          break;
        //case '|':
        //   printf("unsupported or\n");
        //   exit(1);
        //   break;
        default:
          cur = (unsigned char)bsat[i];
          pos[dec(cur)] = 1;
      }
    }

   unsigned long int and = 0;
   // this shouldn't be higher than 125
   for (i=0; i < 125; i++) {
      //printf("pos = %d\tneg = %d\n", pos[i], neg[i]);
      and |= pos[i] & neg[i];
   }

   clock_t end = clock();

   printf("pos & neg = %lu\n", and);
   if (!and)
     printf("Solves");
   else
     printf("no go :(");

   double time_spent = (double)(end - begin) / CLOCKS_PER_SEC;

   printf("\ntime taken = %f", time_spent);

   printf("\n");

   mem_used();

   return 0;
}
