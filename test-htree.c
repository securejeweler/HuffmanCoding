/**************************************************************************/
/*              COPYRIGHT Carnegie Mellon University 2020                 */
/* Do not post this file or any derivative on a public site or repository */
/**************************************************************************/


/* Huffman coding
 *
 * Main file for testing is_htree
 * 15-122 Principles of Imperative Computation
 */

#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <ctype.h>

#include "lib/xalloc.h"
#include "lib/contracts.h"

#include "htree.h"

struct htree_node {
  symbol_t value;
  unsigned int frequency;
  htree *left;
  htree *right;
};

bool is_htree(htree *H);
bool is_htree_leaf(htree *H);
bool is_htree_interior(htree *H);


int main () {
  // Write tests for is_tree here
  htree *thing = xmalloc(sizeof(htree));
  thing->frequency = 11;

  htree *thing2 = xmalloc(sizeof(htree));
  thing2->frequency = 4;
  thing->left = thing2;

  htree *thing3 = xmalloc(sizeof(htree));
  thing3->frequency = 7;
  thing->right = thing3;

  htree *thing4 = xmalloc(sizeof(htree));
  thing4->frequency = 3;
  thing3->left = thing4;

  htree *thing5 = xmalloc(sizeof(htree));
  thing5->frequency = 4;
  thing3->right = thing5;

  htree *thing6 = xmalloc(sizeof(htree));
  thing6->frequency = 2;
  htree *thing7 = xmalloc(sizeof(htree));
  thing7->frequency = 2;

  thing5->left = thing6;
  thing5->right = thing7;

  htree *thing8 = xmalloc(sizeof(htree));
  thing8->frequency = 1;
  htree *thing9 = xmalloc(sizeof(htree));
  thing9->frequency = 1;
  thing6->left = thing8;
  thing6->right = thing9;

  htree *thing10 = xmalloc(sizeof(htree));
  thing10->frequency = 1;
  htree *thing11 = xmalloc(sizeof(htree));
  thing11->frequency = 1;
  thing7->left = thing10;
  thing7->right = thing11;

  ASSERT(is_htree(thing));
  free(thing);
  free(thing2);
  free(thing3);
  free(thing4);
  free(thing5);
  free(thing6);
  free(thing7);
  free(thing8);
  free(thing9);
  free(thing10);
  free(thing11);
  printf("Success!\n");
  return 0;
}
