#include <stdio.h>
#include <stdlib.h>
#include "lookup_table.h"
#include <time.h>
#include <string.h>

unsigned long int do_bsat(char *b) {

  register int i;
  register unsigned long int pos = 0;
  register unsigned long int neg = 0;
  register unsigned char cur = 0;

  for (i=0; i < strlen(b); i++) {
     cur = (unsigned char)b[i];
     switch(cur) {
       case '!':
         cur = (unsigned char)b[++i];
         neg |= lookup[dec(cur)];
         break;
       default:
         cur = (unsigned char)b[i];
         pos |= lookup[dec(cur)];
     }
  } // for strlen(b)

  return pos & neg;

} // void bsat

int main() {

   register int i = 0;
   int chk = 0;
   //   ^* doesn't work, [] does
   //   sizeof gets null?
   char bsat[5000];
   //printf("gimme bsat: ");
   //gets(bsat);

   printf("reading file: binput\n");

   FILE *in_file = fopen("binput", "rb");
   fseek(in_file, 0, SEEK_END);
   ssize_t file_len = ftell(in_file);
   fseek(in_file, 0, SEEK_SET);

   char *whole = malloc(sizeof(char) * file_len );
   memset(whole, 0x0, sizeof(char) * file_len );
   if ( fread(whole, 1, file_len, in_file) != file_len) {
     printf("error reading file\n");
     free(whole);
     exit(1);
   }

   char *line = NULL;
   size_t len = 0;
   ssize_t read;

   line = strtok(strdup(whole), "\n");
   clock_t begin = clock();
   //while( (read = getline(&line, &len, in_file)) != -1) {
   while(line) {
      chk = do_bsat(line);
      line = strtok(NULL, "\n");
      /*
      //printf("chk = %lu\n", chk);
      if (!chk)
        printf("true : %lu\n", chk);
      else
        printf("false: %lu\n", chk);
      */
   }
/*
// read line by line
   FILE *in_file = fopen("binput", "rb");
   char *line = NULL;
   size_t len = 0;
   ssize_t read;

   clock_t begin = clock();
   while( (read = getline(&line, &len, in_file)) != -1) {
      chk = do_bsat(line);

      //printf("chk = %lu\n", chk);
      if (!chk)
        printf("true : %lu\n", chk);
      else
        printf("false: %lu\n", chk);

   }
*/
   clock_t end = clock();

   fclose(in_file);
   free(whole);

   //printf("\npos       = %lu\n", pos);
   //printf("      neg = %lu\n", neg);

   double time_spent = (double)(end - begin) / CLOCKS_PER_SEC;

   printf("\ntime taken = %f", time_spent);
   printf("\n");


// mem use///////////////////////////
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
// mem use///////////////////////////

   printf("mem used = %f MB\n", peak_kb/(double)1024);

   return 0;
}
