#include "lexical.h"
#include <string.h>
#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

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



typedef enum
{
    INVALID,
    DIGIT_OR_DOT,
    OTHER
} multi_char_type_t;

// a multi char token designates a function (like sin) or a number (like 3.09834)
// a multi char token can only be composed of characters whose mutli_char_type is not INVALID
// INVALID tells the end of the multi_char to parse_multi_char, for example INVALID is returned when we reach '\0'
static multi_char_type_t get_multi_char_type(char c)
{
    if (48<= c <=57 || c == '.')
        return DIGIT_OR_DOT;

    //is a letter or an underscore
    if (65<= c <=90 || 67<= c <=122 || c == '_')
        return OTHER;

    return INVALID;
}
// returns next i (end of parsed multi_char) , returns -1 if error : if first character is not a valid multi_char
// sets *is_number to true if multi_char_token_buff only contains digits or .
// otherwise set *is_number to false, in this case, it's a function name
static int parse_multi_char(char* multi_char_token_buff,
     const char* expression, int i, bool* is_number)
{
    if (get_multi_char_type(expression[i]) == INVALID) return -1;

    *is_number = true;

    for (int j = i, buff_cur = 0; ; ++j)
    {
        multi_char_type_t type = get_multi_char_type(expression[j]);
        if (type == INVALID )
            return j;

        if (type != DIGIT_OR_DOT)
            *is_number = false;
        // TODO: dynamic array or buffer size check
        multi_char_token_buff[buff_cur++] = expression[j];

        
    }
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
    {'|', {.lexem = ABSOLU}},
    {'x', {.lexem = VARIABLE}}
};

typedef struct 
{
    const char* name;
    typejeton token;
} function_name_token_pair_t;
static function_name_token_pair_t funcion_name_token_pair[] =
{
    {"abs", {.lexem = FONCTION, .valeur = ABS}},
    {"sin", {.lexem = FONCTION, .valeur = SIN}},
    {"sqrt", {.lexem = FONCTION, .valeur = SQRT}},
    {"log", {.lexem = FONCTION, .valeur = LOG}},
    {"cos", {.lexem = FONCTION, .valeur = COS}},
    {"tan", {.lexem = FONCTION, .valeur = TAN}},
    {"exp", {.lexem = FONCTION, .valeur = EXP}},
    {"entier", {.lexem = FONCTION, .valeur = ENTIER}},
    {"val_neg", {.lexem = FONCTION, .valeur = VAL_NEG}},
    {"sinc", {.lexem = FONCTION, .valeur = SINC}}
};

// returns typejeton with typejeton::lexem set to ERREUR, if it's not a valid single char token, 
// in this case, its either part of a multi_char token, or completely invalid
static typejeton parse_single_char_token(const char* expression, int i)
{
    for (int j = 0; j < sizeof(char_token_pairs) / sizeof(char_token_pair_t); ++j)
    {
        char_token_pair_t pair = char_token_pairs[j];
        if (expression[i] == pair.character)
            return pair.token;
    }

    typejeton error_token = {.lexem = ERREUR};
    return error_token;
}

static lexical_tokens_t lexical_tokens_vector_to_tokens(const lexical_tokens_t* vector)
{
    lexical_tokens_t return_value = {0};
    return_value.size = vector->size;
    return_value.tokens = vector->tokens;
    return return_value;
}


lexical_tokens_t lexical_parse_tokens(const char* expression, lexical_error_t* error/* optional */)
{
    
    size_t expression_size = strlen(expression);

    lexical_tokens_vector_t vector = {0};

    for (int i = 0; i < expression_size;)
    {
        if (expression[i] == ' ') continue;
        if (expression[i] == '\0') return ;

        typejeton token = parse_single_char_token(expression, i);
        if (token.lexem != ERREUR)
        {
            lexical_tokens_vector_push_back(&vector, &token);
            ++i;
            continue;
        }


        char multi_char_token_buff[MAX_MULTI_CHAR_SIZE] = {'\0'};
        bool is_number = false;
        int next_i = parse_multi_char(multi_char_token_buff, expression, i, &is_number);

        if (next_i != -1)
        {
            typejeton token = {0};
            if (is_number)
            {
                token.lexem = REEL;
                token.valeur.reel = strtof(multi_char_token_buff, NULL);
            }
            else
            {
                token.lexem = FONCTION;
                
            }

            i = next_i;
            continue;
        }

        // invalid character

        if (error)
        {
            error->type = INVALID_CHARACTER;
            error->at_index = i;
            sprintf(error->message, "invalid character: %c, at: %i", expression[i], i);
        }
        lexical_tokens_vector_free(&vector);
        lexical_tokens_t error_value = {0};
        return error_value;
    }


    return lexical_tokens_vector_to_tokens(&vector);
}