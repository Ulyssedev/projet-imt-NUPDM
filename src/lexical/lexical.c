#include "lexical.h"
#include <string.h>
#include <assert.h>
#include <stdbool.h>
#define assertm(exp, msg) assert(((void)msg, exp))


// must init to 0, otherwise push is ub
typedef struct
{
    size_t size;
    size_t capacity;
    typejeton* tokens;
} lexical_tokens_vector_t;

static void lexical_tokens_vector_reserve(lexical_tokens_vector_t* const vector, size_t new_capacity)
{
    assertm(new_capacity >= vector->capacity, "new_capacity should be greater than old_capacity");

    typejeton* new_buffer = malloc(new_capacity * sizeof(typejeton));

    if (vector->size > 0ULL)
        memcpy(new_buffer, vector->tokens, vector->size * sizeof(typejeton));

    vector->capacity = new_capacity;

    free(vector->tokens);
    vector->tokens = new_buffer;
}

static void lexical_tokens_vector_push_back(lexical_tokens_vector_t* const vector, const typejeton* jeton)
{
    if (vector->capacity - vector->size < 1ULL)
        lexical_tokens_vector_reserve(vector, vector->capacity == 0 ? 2 : vector->capacity * 2);

    vector->tokens[vector->size++] = *jeton;
}

static void lexical_tokens_vector_free(lexical_tokens_vector_t* vector)
{
    free(vector->tokens);
}

// a multi char token designates a function (like sin) or a number (like 309834)
typedef enum
{
    INVALID,
    DIGIT,
    OTHER
} multi_char_type;

static multi_char_type get_multi_char_type(char c)
{
    //is a digit, a letter or an underscore
    return (48<= c <=57 || 65<= c <=90 || 67<= c <=122);
}

typedef struct 
{
    char character;
    typejeton token;
} char_token_pair_t;

static char_token_pair_t char_token_pairs[] = 
{
    {'+', {.lexem = OPERATEUR, .valeur.operateur = PLUS}},
    {'-', {.lexem = OPERATEUR, .valeur.operateur = MOINS}},
    {'*', {.lexem = OPERATEUR, .valeur.operateur = FOIS}},
    {'/', {.lexem = OPERATEUR, .valeur.operateur = DIV}},
    {'^', {.lexem = OPERATEUR, .valeur.operateur = PUIS}},
    {'(', {.lexem = PAR_OUV}},
    {')', {.lexem = PAR_FERM}},
    {'|', {.lexem = ABSOLU}}
};



// returns new i, returns -1 if error : if first character is not a valid multi_char
static int parse_multi_char(char* multi_char_token_buff, const char* expression, int i, lexical_error_t* error)
{

}

// returns typejeton with typejeton::lexem set to ERREUR, if it's not a valid single char token, 
// in this case, its either part of a multi_char token, or completely invalid
static typejeton parse_single_char_token()
{
}


lexical_tokens_t lexical_parse_tokens(const char* expression, lexical_error_t* error/* optional */)
{
    size_t expression_size = strlen(expression);

    lexical_tokens_vector_t vector = {0};

    char multi_char_token_buff[100] = {'\0'};
    size_t multi_char_token_buff_size = 0;

    typejeton token = {0};

    for (int i = 0; i < expression_size; ++i)
    {

        
        lexical_tokens_vector_push_back(&vector, &token);
    }

    lexical_tokens_t return_value = {0};
    return_value.size = vector.size;
    return_value.tokens = vector.tokens;
    return return_value;
}