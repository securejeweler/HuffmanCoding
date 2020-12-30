/**************************************************************************/
/*              COPYRIGHT Carnegie Mellon University 2020                 */
/* Do not post this file or any derivative on a public site or repository */
/**************************************************************************/


/* Huffman coding
 *
 * 15-122 Principles of Imperative Computation
 */

#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <ctype.h>

#include "lib/contracts.h"
#include "lib/xalloc.h"
#include "lib/file_io.h"
#include "lib/heaps.h"

#include "freqtable.h"
#include "htree.h"
#include "encode.h"
#include "bitpacking.h"

// Print error message
void error(char *msg) {
  fprintf(stderr, "%s\n", msg);
  exit(1);
}

/* in htree.h: */
/* typedef struct htree_node htree; */
struct htree_node {
  symbol_t value;
  unsigned int frequency;
  htree *left;
  htree *right;
};

/**********************************************/
/* Task 1: Checking data structure invariants */
/**********************************************/

/* forward declaration -- DO NOT MODIFY */
bool is_htree(htree *H);

bool is_htree_leaf(htree *H) {
  // WRITE ME
  if(H == NULL) return false;
  if(H->frequency <= 0) return false;
  ASSERT(H->frequency > 0);
  return H->left == NULL && H->right == NULL;
}

bool is_htree_interior(htree *H) {
  // WRITE ME
  if(H == NULL) return false;
  if(H->left == NULL || H->right == NULL) return false;
  if(!is_htree(H->left) || !is_htree(H->right)) return false;

  return H->left->frequency + H->right->frequency == H->frequency;
}

bool is_htree(htree *H) {
  // WRITE ME
  if(H == NULL) return false;
  ASSERT(H != NULL);
  return is_htree_leaf(H) || is_htree_interior(H);
  
}


/********************************************************/
/* Task 2: Building Huffman trees from frequency tables */
/********************************************************/

/* for libs/heaps.c */
bool htree_higher_priority(elem e1, elem e2) {
  // WRITE ME
  return ((htree*) (e1))->frequency < ((htree*)(e2))->frequency;
}


// build a htree from a frequency table
htree* build_htree(freqtable_t table) {
  unsigned int nonzero = 0;
  
  REQUIRES(is_freqtable(table));
  pq_t  q = pq_new(NUM_SYMBOLS,&htree_higher_priority,NULL);
  for(unsigned int c = 0; c < NUM_SYMBOLS; c++){
    if(table[c] != 0){
      nonzero += 1;
      htree* tree = xmalloc(sizeof(htree));
      tree->frequency =  table[(symbol_t)c];
      tree->value = (symbol_t)c;
      tree->left = NULL;
      tree->right = NULL;
      pq_add(q,(elem)tree);
    }
    
  }

  if(nonzero < 2){
    char *msg = "less than two symbols";
    error(msg);
  }
  
  
  unsigned int counter = 1;
  while(counter < nonzero ){
    
    htree* tree = (htree*) (pq_rem(q));
    htree* tree2 = (htree*) (pq_rem(q));
    htree* added_tree = xmalloc(sizeof(htree));
    added_tree->frequency = tree->frequency + tree2->frequency;
    added_tree->value = 'f';
    added_tree->left = tree;
    added_tree->right = tree2;
    pq_add(q,(elem)added_tree);
    counter += 1;
  }
  htree* temp = (htree*) pq_peek(q);
  pq_free(q);
  return temp;

}


/*******************************************/
/*  Task 3: decoding a text                */
/*******************************************/

// Decode code according to H, putting decoded length in src_len
symbol_t* decode_src(htree *H, bit_t *code, size_t *src_len) {
  // WRITE ME
  //print_htree(H);
  //ASSERT(is_htree_leaf(H->left));
  int counter = 0;
  unsigned int index = 0;
  htree *curr = H;
  while(code[index] != '\0'){
    if(is_htree_leaf(curr)) {
      counter += 1;
      curr = H;
    }
    else if(code[index] == '0'){
      curr = curr->left;
      index += 1;
    }
    else {
      ASSERT(code[index] == '1');
      curr = curr->right;
      index += 1;
    }  
  }
  if(is_htree_leaf(curr)) {
      counter += 1;
      curr = H;
  }
  //printf("%d",curr->frequency);
  
  if(code[index] == '\0' && curr != H){
    char* msg = "invalid decode";
    error(msg);
  }

  symbol_t *res = xmalloc(sizeof(symbol_t) * (counter + 1));

  unsigned int indexbit = 0;
  unsigned int resindex = 0;
  htree *curr2 = H;
  while(code[indexbit] != '\0'){
    if(is_htree_leaf(curr2)) {
      res[resindex] = curr2->value;
      
      resindex += 1;
      curr2 = H;
    }
    //has two children now i think? dk
    else if(code[indexbit] == '0'){
      curr2 = curr2->left;
      indexbit += 1;
    }
    else {
      ASSERT(code[indexbit] == '1');
      curr2 = curr2->right;
      indexbit += 1;
    }
  }
  if(is_htree_leaf(curr2)) {
      res[resindex] = curr2->value;
      resindex += 1;
      curr2 = H;
  }
  res[counter] = '\0';
  *src_len = counter;
  return res; // bogus
}


/****************************************************/
/* Tasks 4: Building code tables from Huffman trees */
/****************************************************/

void helper(htree *curr, codetable_t table, char *code, int top)
{
  
  if(curr->left != NULL){
    code[top] = '0';
    helper(curr->left,table,code,top+1);
  }

  if(curr->right != NULL){
    code[top] = '1';
    helper(curr->right,table,code,top+1);
  }

  if(is_htree_leaf(curr))
  {
    table[curr->value] = xmalloc((strlen(code) + 1) * sizeof(bit_t));
    for(int i = 0; i < top; i++){
      table[curr->value][i] = code[i];
    }
    table[curr->value][top] = '\0';
  }
}

// Returns code table for characters in H
codetable_t htree_to_codetable(htree *H) {
  // WRITE ME
  codetable_t res = xmalloc(NUM_SYMBOLS * sizeof(bitstring_t));
  for(unsigned int i = 0; i < NUM_SYMBOLS; i++){
    res[i] = NULL;
  }
  htree* curr = H;
  char *temp = xmalloc(sizeof(char) * 256);
  for(int i = 0; i < NUM_SYMBOLS; i++){
    temp[i] = '\0';
  }
  
  helper(curr,res,temp,0);
  free(temp);
  return res;
}


/*******************************************/
/*  Task 5: Encoding a text                */
/*******************************************/

// Encodes source according to codetable
bit_t* encode_src(codetable_t table, symbol_t *src, size_t src_len) {
  // WRITE ME
  unsigned int counter = 0;
  for(unsigned int i = 0; i < src_len;i++)
  {
    if(table[src[i]] == NULL){
      char* msg = "invalid encode";
      error(msg);
    }
    ASSERT(table[src[i]] != NULL);
    counter += strlen(table[src[i]]);
  }

  bit_t *res = xmalloc(sizeof(bit_t)*(counter + 1));
  unsigned int bitstring = 0;
  for(unsigned int i = 0;i < src_len; i++ ){
    int j = 0;
    while(table[src[i]][j] != '\0'){
      res[bitstring] = table[src[i]][j];
      j += 1;
      bitstring += 1;
    }
  }
  res[counter] = '\0';
  return res; // bogus
}


/**************************************************/
/*  Task 6: Building a frequency table from file  */
/**************************************************/

// Build a frequency table from a source file (or STDIN)
freqtable_t build_freqtable(char *fname) {
  // WRITE ME
  freqtable_t res = xcalloc(sizeof(unsigned int), NUM_SYMBOLS);
  FILE *thing = fopen(fname,"r");
  int i;
  while((i = fgetc(thing)) != EOF )
  {
    res[i] += 1;
  }
  fclose(thing);
  return res;
}



/************************************************/
/*  Task 7: Packing and unpacking a bitstring   */
/************************************************/

// Pack a string of bits into an array of bytes; length = strlen(bits)/8
uint8_t* pack(bit_t *bits) {
  // WRITE ME
  unsigned int totarr = num_padded_bytes(strlen(bits));
  uint8_t *res2 = xmalloc(totarr * sizeof(uint8_t));
  int startindex = (totarr * 8) - 1;
  int comparindex = strlen(bits);
  for(int i = totarr - 1; i > -1; i=i-1)
  {
    int temp = startindex - 8;
    int bi = 0;
    unsigned char overall = 0;
    while(startindex > temp)
    {
      if(startindex < comparindex)
      {
        if(bits[startindex] == '1') overall = overall | (1 << bi);
        //printf("%u\n",overall);
      }
      startindex -= 1;
      bi += 1;
    }
    res2[i] = overall;
    
  }
  return res2;
}

// Unpack an array of bytes c of length len into a NUL-terminated string of ASCII bits
bit_t* unpack(uint8_t *c, size_t len) {
  // WRITE ME
  //int stringsize =  len * 8;
  char *res = xcalloc((8*len) + 1, 1);

  for(unsigned int i = 0; i < len; i++)
  {
    int j = 7;
    while(j >=0)
    {
      int offset = 8*i;
      int num = (c[i] & (1 << j));
      res[offset + (7 - j)] = '0' + (num >> j);
      j -= 1;
    }
  }

  res[8*len] = '\0';
  return res;
}
