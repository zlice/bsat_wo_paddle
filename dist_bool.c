#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "lookup_table.h"
#include "reverse_lookup.h"

typedef struct p_tree {
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
int untie(char *in, unsigned long int *nin) {
//void untie(char *in, unsigned char *nin) {
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
  // #define ADD_NIN(n) nin[j++] = n;
  // machine version
  #define ADD_NIN(n) nin[j++] |= lookup[dec(n)];
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

// actually distribute and assign values
void do_dist(chain *or, p_tree p, int e) {

  int s;
  // printf("   dist(ch, %d, %d)\n", p.start, e);
  // printf("   dist ___ 0x%llx\n", p.pos_dist);
  // printf("   dist ___ 0x%llx\n", p.neg_dist);
  for (s=p.start; s <= e; s++) {
    //printf("*");
    or[s].pos |= p.pos_dist;
    or[s].neg |= p.neg_dist;
  }
  p.pos_dist  = 0;
  p.neg_dist  = 0;
  //printf("\n");

} // void do_dist

// loop over binary input and distribute chains
// TODO: layer 0 | ???
void dist(unsigned long int *in, unsigned int *c_szs, chain *or_ch, p_tree *p_layers) {
            //input,              chain_szs,    or_chains,         p_layers

  //          in_pos -   or_tot/c_szs[0] - layer pos - or_ch pos
  unsigned int     i = 0,    csz_pos = 1,      l_pos = 0, chain_pos = 0;
  int is_disting = 1;
  unsigned long int cur_tok = 0, cur_p = 0, cur_n = 0;

  //for (i=0; i < in_sz ; i++) {
  while (in[i] != 0x0) {
    cur_tok = in[i];
    switch (cur_tok) {
      case 1: // (
         p_layers[l_pos].start = chain_pos;
         p_layers[l_pos].pos_dist |= cur_p;
         p_layers[l_pos].neg_dist |= cur_n;
         cur_p = cur_n = is_disting = 0;
         l_pos++;
         break;
      case 2: // )
         if (is_disting == 1) { // NOT (x|x)
            p_layers[l_pos].pos_dist |= cur_p;
            p_layers[l_pos].neg_dist |= cur_n;
         } else {
            or_ch[chain_pos].pos |= cur_p;
            or_ch[chain_pos].neg |= cur_n;
            chain_pos++;
            //printf(") inc pos\n");
         }
         is_disting = 1;
         cur_p = cur_n = 0;
         l_pos--;
         if (l_pos == 0) { // back out to layer 0
           csz_pos++;
           is_disting = 0; // ok bc ( assumes disting
         } else {
           do_dist(or_ch, p_layers[l_pos], chain_pos);
         }
         break;
      case 4: // |
         if (l_pos > 0) // need to add layer 0
           c_szs[csz_pos]++;
         if (is_disting == 1) {
            p_layers[l_pos].pos_dist |= cur_p;
            p_layers[l_pos].neg_dist |= cur_n;
            do_dist(or_ch, p_layers[l_pos], chain_pos);
         } else {
            or_ch[chain_pos].pos |= cur_p;
            or_ch[chain_pos].neg |= cur_n;
            chain_pos++;
            //printf("| inc pos\n");
         }
         cur_p = cur_n = is_disting = 0; // has to be x|x
         p_layers[l_pos].start = chain_pos + 1; //??????????
         break;
      case 8: // !
         // these should only be !x now
         cur_tok = in[++i];
         cur_n |= cur_tok;
         break;
      default:  // cur_tok > 16
         cur_p |= cur_tok;
         break;
    } // switch(cur_tok)
    i++;
  } // while strlen(in)

  // wrap up
  //printf("wrap up\n");
  register int n = 1;
  csz_pos--;
  c_szs[csz_pos+1] = 0;
  for (n=1; n <= csz_pos; n++) {
    c_szs[csz_pos+2] *= c_szs[n]; // max expand
    c_szs[csz_pos+1] += c_szs[n]; // real tot or_chains
  }
  c_szs[0] = csz_pos; // number of (|)

  // final dist for layer 0
  p_layers[0].start = 0; // without layer 0 |, dist everywhere
  do_dist(or_ch, p_layers[0], chain_pos - 1);

} // void dist

chain expand(long unsigned int *zero_solves, chain *or_ch, unsigned int *c_szs, int or_start, int csz_idx, chain dist) {

  chain cur_ch; // added up dist, passed recurisvely
  cur_ch.pos = 0;
  cur_ch.neg = 0;
  int cur_sz = c_szs[csz_idx];
  register int n;

  for (n=0 ; n < cur_sz ; n++) {
    cur_ch.pos = or_ch[or_start + n].pos | dist.pos;
    cur_ch.neg = or_ch[or_start + n].neg | dist.neg;
    if (csz_idx < c_szs[0]) { // max or-block (||)(||)(|)
      chain tmp_ch = expand(zero_solves, or_ch, c_szs, or_start + cur_sz, csz_idx + 1, cur_ch);
      cur_ch.pos |= tmp_ch.pos;
      cur_ch.neg |= tmp_ch.neg;
    } else {
      //zero_solves[c_szs[c_szs[0] + 2]-- - 1 ] = (cur_ch.pos | dist.pos) & (cur_ch.neg | dist.neg);
      //zero_solves[0] = (cur_ch.pos | dist.pos) & (cur_ch.neg | dist.neg);
      // don't need array at all? just print solves? takes time but...way less mem
      unsigned long int chk = (cur_ch.pos | dist.pos) & (cur_ch.neg | dist.neg);
      if (chk == 0) {
        // reverse lookup - output what the AND chains are per line
        //printf("chk = 0x%llx\n", (cur_ch.pos | dist.pos) & (cur_ch.neg | dist.neg) );

        register unsigned long int yes = cur_ch.pos | dist.pos;
        register unsigned long int not = cur_ch.neg | dist.neg;
        register int b = 0;
        register unsigned long int uno = 0x4000000000000000;//4611686018427387904;
        for (b=0; b < 59; b++) {
          //printf("chk = 0x%llx\n", yes | not); // ????
          if (uno & yes) printf("%c", rev_table[b]);
          if (uno & not) printf("!%c", rev_table[b]);
          yes = yes << 1;
          not = not << 1;
        }
        printf("\n");
      } // reverse lookup

    } // else
  } // for n < cur_sz

  return cur_ch;
} // unsigned long int expand

int main() {

  //char *prob = "a && (b || c) || d";
  //char *prob = "a && !(b || c) || d";
  //char *prob = "a && (!b || c) || d"; // works
  //char *prob = "a && !(!(b || c) || d)";
  //char *prob = "a && !(!(b || !(c && e)) || d)";
  char prob[2000] = {0};
  printf("bsat : ");
  gets(prob);
  unsigned long int new_prob[2000] = {0}; // sizeof(prob)
  //unsigned char new_prob[1000] = {0}; // sizeof(prob)

  unsigned int i = 0;
  // untie
  // remove !()s - convert to binary
  clock_t untie_begin = clock();
  int or_tot = untie(prob, new_prob);
  clock_t untie_end = clock();

  // human text
  // printf("%s\n=\n%s\n", prob, new_prob);

  // hex text
  //printf("%s\n=\n", prob);

/*
  while (new_prob[i] != 0x0) // work? w/ 0
    printf("0x%llx - ", new_prob[i++]);
  printf("\n");
*/
  // these are passed to distributor
  // or chain size array, [0] is max/or_tot, [max+1] tot_chains
  // figured out latter by (||) cnt + 1
  printf("chain_sz malloc\n");
  unsigned int *chain_szs = malloc(sizeof(unsigned int) * (or_tot + 2) );
  chain_szs[0] = or_tot;
  i = 1;
  while (i < chain_szs[0] + 2) // bc memset()
    chain_szs[i++] = 1;

  printf("or_ch malloc\n");
  chain  *or_chains = malloc(sizeof(chain) * (or_tot * 2) );
  printf("p_layer malloc\n");
  p_tree *p_layers  = malloc(sizeof(p_tree) * (or_tot * 2) );

  memset(or_chains, 0x0, sizeof(chain) * (or_tot * 2) );
  memset(p_layers,  0x0, sizeof(p_tree) * (or_tot * 2) );

  clock_t dist_begin = clock();
  dist(new_prob, chain_szs, or_chains, p_layers);
  clock_t dist_end = clock();

  i = 0;
  while (i < chain_szs[0] + 3) { // (|) + cnt + tot + exp
    printf("chain_szs[%lu] = %lu, ", i, chain_szs[i]);
    i++;
  }
/*
  i = 0;
  while (i < chain_szs[chain_szs[0] + 1]) {
    printf("\nor_chains[%d]\n", i);
    printf("   pos = 0x%llx\n", or_chains[i].pos);
    printf("   neg = 0x%llx\n", or_chains[i].neg);
    //printf("\n");
    i++;
  }
*/
  printf("zero_solves no-malloc\n");
  chain dist;
  dist.pos = 0;
  dist.neg = 0;
  unsigned long int *zero_solves = malloc(8);
  memset(zero_solves, 0x0, 8);
  //unsigned long int *zero_solves = malloc(sizeof(unsigned long int) * chain_szs[chain_szs[0] + 2]);
  //memset(zero_solves, 0x0, sizeof(unsigned long int) * chain_szs[chain_szs[0] + 2]);
  unsigned int csz_sv = chain_szs[chain_szs[0] + 2];

  clock_t expand_begin = clock();
  expand(zero_solves, or_chains, chain_szs, 0, 1, dist);
  clock_t expand_end = clock();
/*
  i = 0;
  //while (i < chain_szs[chain_szs[0] + 2]) {
  while (i < csz_sv) {
    if (zero_solves[i] == 0) {
      printf("solved by and_chain[%d]\n", i);
      //printf("   pos = 0x%llx\n", or_chains[i].pos);
      //printf("   neg = 0x%llx\n", or_chains[i].neg);
    } else {
      printf("BAD solution %d = %d\n", i, zero_solves[i]);
    }
    i++;
  }
*/
  double untie_time = (double)(untie_end - untie_begin) / CLOCKS_PER_SEC;
  double dist_time = (double)(dist_end - dist_begin) / CLOCKS_PER_SEC;
  double expand_time = (double)(expand_end - expand_begin) / CLOCKS_PER_SEC;
  printf("\n");
  printf("untie took : %f\n", untie_time);
  printf("dist took : %f\n", dist_time);
  printf("expand took : %f\n", expand_time);
  printf("\ntotal time : %f\n", untie_time + dist_time + expand_time);
  printf("\n");

  //mem_used()/////////////////////////////////////////////////////
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
  //mem_used()/////////////////////////////////////////////////////

  free(chain_szs);
  free(or_chains);
  free(p_layers);
  free(zero_solves);

  return 0;

}
