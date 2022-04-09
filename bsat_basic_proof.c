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
/*
   for(i=0; i < 255; i++) {
     printf("lookup[%d] = %lu;\n", i, lookup[i] );
   }
   //printf("lookup[4] = %lu\n", lookup[dec('5')] );
*/
   register unsigned long int pos = 0;
   register unsigned long int neg = 0;
   //unsigned long int pn[2] = {0, 0};
   //int not = 0;
   register unsigned char cur = 0;
   //char bsat[] = "a!bcd!ae";
   //char bsat[] = "a!bcdae";
   //char bsat[] = "a!bcdaeb";
   //char bsat[] = "a!bcdea!e";
   //char bsat[] = "a!bcdae";
   //char bsat[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ987654!b";
   //char bsat[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ987654";
   /*
    * sizeof bsat = 59
    * pos & neg = 0
    * Solves
    * time taken = 0.000000
    * mem used = 2.253906 MB
   */
   //
   //char bsat[] = "!a!b!c!d!e!f!g!h!i!j!k!l!m!n!o!p!q!r!s!t!u!v!w!x!y!z!A!B!C!D!E!F!G!H!I!J!K!L!M!N!O!P!Q!R!S!T!U!V!W!X!Y!Z!9!8!7!6!5!4";
   //char bsat[] = "a!a!b!c!d!e!f!g!h!i!j!k!l!m!n!o!p!q!r!s!t!u!v!w!x!y!z!A!B!C!D!E!F!G!H!I!J!K!L!M!N!O!P!Q!R!S!T!U!V!W!X!Y!Z!9!8!7!6!5!4";
   /*
    * sizeof bsat = 118
    * pos & neg = 32
    * no go :(
    * time taken = 0.000003
    * mem used = 2.253906 MB
    */
   //char bsat[] = "!a!b!c!d!e!f!g!h!i!j!k!l!m!n!o!p!q!r!s!t!u!v!w!x!y!z!A!B!C!D!E!F!G!H!I!J!K!L!M!N!O!P!Q!R!S!T!U!V!W!X!Y!Z!9!8!7!6!5!4!a!b!c!d!e!f!g!h!i!j!k!l!m!n!o!p!q!r!s!t!u!v!w!x!y!z!A!B!C!D!E!F!G!H!I!J!K!L!M!N!O!P!Q!R!S!T!U!V!W!X!Y!Z!9!8!7!6!5!4!a!b!c!d!e!f!g!h!i!j!k!l!m!n!o!p!q!r!s!t!u!v!w!x!y!z!A!B!C!D!E!F!G!H!I!J!K!L!M!N!O!P!Q!R!S!T!U!V!W!X!Y!Z!9!8!7!6!5!4!a!b!c!d!e!f!g!h!i!j!k!l!m!n!o!p!q!r!s!t!u!v!w!x!y!z!A!B!C!D!E!F!G!H!I!J!K!L!M!N!O!P!Q!R!S!T!U!V!W!X!Y!Z!9!8!7!6!5!4";
   //   ^* doesn't work, [] does
   //   sizeof gets null?
   char bsat[5000];
   printf("gimme bsat: ");
   gets(bsat);
   printf("sizeof bsat = %d\n", strlen(bsat) );
   //cur = (unsigned char)bsat[0];

   clock_t begin = clock();


// for
/*
      for (i=0; i < strlen(bsat) ; i++ ) {
        cur = (unsigned char)bsat[i];
        if (cur == dec('!') ) {
            i++;
            cur = (unsigned char)bsat[i];
            neg |= lookup[dec(cur)];
         }
         else {
             pos |= lookup[dec(cur)];
        }
        //printf("%d-", i);
      }
*/
// this currently seems most balanced

     for (i=0; i < strlen(bsat); i++) {
        cur = (unsigned char)bsat[i];
        //printf("%d-", i);
        //printf("%c-",cur);
        switch(cur) {
          case '!':
            cur = (unsigned char)bsat[++i];
            neg |= lookup[dec(cur)];
            break;
          //case '|':
          //   printf("unsupported or\n");
          //   exit(1);
          //   break;
          default:
            cur = (unsigned char)bsat[i];
            //printf("wtf%c-",cur);
            pos |= lookup[dec(cur)];
            //break;
        }
      }

// this is ok but dumb
// and unbalanced, pos good, neg meh
/*
     for (i=0; i < strlen(bsat) - 1; i++) {
        switch(cur) {
          case '!':
            not = 1;
            break;
          //case '|':
          //   printf("unsupported or\n");
          //   exit(1);
          //   break;
          default:
             cur = (unsigned char)bsat[i];
             pn[not] |= lookup[dec(cur)];
             break;
        }
        //printf("%d-", i);
      }
// array flag just seems to make horrible code
*/

/* this is trash, repxx instr
      for (i=0; i < strlen(bsat) ; i++ ) {
        cur = (unsigned char)bsat[i];
        if (cur == dec('!') )
            not = 1;
         else
            pn[not] |= lookup[dec(cur)];
        //printf("%d-", i);
      }
*/

      unsigned long int and = pos & neg;

      clock_t end = clock();

      printf("\npos       = %lu\n", pos);
      printf("      neg = %lu\n", neg);

      printf("pos & neg = %lu\n", and);
      if (!and)
        printf("Solves");
      else
        printf("no go :(");

      double time_spent = (double)(end - begin) / CLOCKS_PER_SEC;

      printf("\ntime taken = %f", time_spent);

      printf("\n");

      //mem_used();

   return 0;
}
