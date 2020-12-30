/**************************************************************************/
/*              COPYRIGHT Carnegie Mellon University 2020                 */
/* Do not post this file or any derivative on a public site or repository */
/**************************************************************************/


/* Huffman coding
 *
 * Main file for testing pack and unpack
 * 15-122 Principles of Imperative Computation
 */

#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <ctype.h>

#include "encode.h"
#include "bitpacking.h"


// You are welcome to define helper functions for your tests

int main () {
  // Create a few NUL-terminated string of ASCII bits and corresponding arrays of bytes

  // Using them, test pack and unpack

  char *s1 = "11010110101011010101010101110101001101";
  //printf("%u",(unsigned int)strlen(s1));
  uint8_t *res = pack(s1);
  for(unsigned int i = 0; i < 5; i++)
  {
    printf("%u\n",res[i]);
  }

  bit_t *thing = unpack(res, 5);
  for(unsigned int i = 0; i<strlen(thing);i++)
  {
    printf("%c",thing[i]);
  }
  printf("All tests passed!\n");
  return 0;
}
