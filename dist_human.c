#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "lookup_table.h"

typedef struct p_tree {
  int layer;              // (()) layer
  int start;              // what | block () starts
  unsigned long int pos_dist; // distribute
  unsigned long int neg_dist; // distribute
} p_tree;

typedef struct chain {
  unsigned long int pos;
  unsigned long int neg;
} chain;

// remove !()s and create new binary string from input
// this makes the next steps easier
int untie(char *in, unsigned char *nin) {
            //input, new input aka translated input

  // this -should- check for 'good' (expected) values
  // also
  // -should not- assume && or ||

  // in_pos -    nin_pos -     total | <-- used for chain sizes
  int     i = 0,       j = 0, or_tot = 1;
  //        cnt -    layer pos
  int not_layer = 0, l_pos = 0;
  register int in_sz = strlen(in);   // less code (1 repnz instr)
  #define is_notting (not_layer % 2) // more effective than a variable
  // human readable
  #define ADD_NIN(n) nin[j++] = n;
  // machine version
  // #define ADD_NIN(n) nin[j++] |= lookup[dec(n)];
  unsigned long int cur_tok = 0;

  for (i=0; i < in_sz ; i++) {
    cur_tok = (unsigned long int)in[i];
    //cur_tok = (unsigned char)in[i];
    switch (cur_tok) {
      case ' ':             continue;
      case '!':
         cur_tok = (unsigned long int)in[++i];
         //cur_tok = (unsigned char)in[++i];
         if (cur_tok == '(') {  // !()
           l_pos++;
           not_layer++;
         } else                 // !x
           ADD_NIN(dec('!') );
         break;
      case '(':
         l_pos++;
         break;
      case ')': // could only dist on ) ? may make more sense, easier
         l_pos--;
         if (l_pos < not_layer)
            not_layer--;
         break;
      case '|':
         i++;   // skip expected next |
         if (is_notting)    continue;
         else               or_tot++;
         break;
      case '&': // only used with !() dist
         i++;   // skip expected next &
         if (is_notting) {
           cur_tok = dec('|');
           or_tot++;
           break;
         }
         continue;
      default:  // cur_tok > 16
         if (is_notting)    ADD_NIN(dec('!') );
         break;
    } // switch(cur_tok)
    ADD_NIN(cur_tok);
    //printf("adding : %s\n", (unsigned char)cur_tok);
  } // for strlen(in)

  return or_tot;
} // int untie

int main() {

  //char *prob = "a && (b || c) || d";
  //char *prob = "a && !(b || c) || d";
  //char *prob = "a && (!b || c) || d"; // works
  //char *prob = "a && !(!(b || c) || d)";
  //char *prob = "a && !(!(b || !(c && e)) || d)";
  char prob[2000] = {0};
  printf("bsat : ");
  gets(prob);
  unsigned char new_prob[1000] = {0}; // sizeof(prob)

  // remove !()s - convert to binary
  int or_tot = untie(prob, new_prob);

  // human text
  // printf("%s\n=\n%s\n", prob, new_prob);

  // hex text
  printf("\n%s\n=\n", prob);
  printf("%s\n", new_prob);

  return 0;

}
