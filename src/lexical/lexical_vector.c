#include "lexical_vector.h"

void lexical_tokens_vector_reserve(lexical_tokens_vector_t *const vector,
                                   size_t new_capacity) {
  if (new_capacity <= vector->capacity)
    return;

  typejeton *new_buffer = malloc(new_capacity * sizeof(typejeton));

  if (vector->size > 0ULL)
    memcpy(new_buffer, vector->tokens, vector->size * sizeof(typejeton));

  vector->capacity = new_capacity;

  free(vector->tokens);
  vector->tokens = new_buffer;
}

void lexical_tokens_vector_push_back(lexical_tokens_vector_t *const vector,
                                     const typejeton *jeton) {
  if (vector->size + 1 > vector->capacity)
    lexical_tokens_vector_reserve(
        vector, vector->capacity == 0 ? 2 : vector->capacity * 2);

  vector->tokens[vector->size++] = *jeton;
}

void lexical_tokens_vector_free(lexical_tokens_vector_t *vector) {
  free(vector->tokens);
}