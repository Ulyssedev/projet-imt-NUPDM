#ifndef LEXICAL_VECTOR_H
#define LEXICAL_VECTOR_H

#include "../common/jeton.h"
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

// must be freed if capacity != 0 !!!
typedef struct {
  size_t size; // actual number of typjeton
  size_t capacity;
  typejeton *tokens;
} lexical_tokens_vector_t;

void lexical_tokens_vector_reserve(lexical_tokens_vector_t *const vector,
                                   size_t new_capacity);
void lexical_tokens_vector_push_back(lexical_tokens_vector_t *const vector,
                                     const typejeton *jeton);
void lexical_tokens_vector_free(lexical_tokens_vector_t *vector);

#endif // LEXICAL_VECTOR_H