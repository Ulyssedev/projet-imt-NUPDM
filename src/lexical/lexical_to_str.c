#include "lexical_vector.h"
#include <stddef.h>

typedef struct
{
    size_t size;
    size_t capacity;
    char*  buff;
} char_vector_t;

static void reserve(char_vector_t* char_vector, size_t new_capacity)
{
    if (new_capacity <= char_vector->size) return;

    
}


static void push_char_buff(char_vector_t* char_vector, const char* buff, size_t buff_size)
{

}

static void push_str(char_vector_t* char_vector, const char* str)
{

}

char * lexical_tokens_to_str(lexical_tokens_vector_t* tokens)
{
    if (!tokens || tokens->size == 0) return "";

    char_vector_t char_vector = {0};

    for (int i = 0; i < tokens-> size; ++i)
    {
        typejeton current_token = tokens->tokens[i];

        
        
    }
}
