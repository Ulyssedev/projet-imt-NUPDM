#include "lexical.h"
#include "lexical_vector.h"

#include <stddef.h>
#include <string.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>

#include <assert.h>

typedef struct
{
    size_t size;
    size_t capacity;
    char*  buff;
} char_vector_t;

static void reserve(char_vector_t* char_vector, size_t new_capacity)
{
    if (new_capacity <= char_vector->capacity) return;

    char* new_buffer = malloc(new_capacity);

    if (char_vector->size > 0ULL)
        memcpy(new_buffer, char_vector->buff, char_vector->size);

    char_vector->capacity = new_capacity;

    free(char_vector->buff);
    char_vector->buff = new_buffer;
}

static void push_char(char_vector_t* char_vector, char c)
{
    if (char_vector->size + 1 > char_vector->capacity)
        reserve(char_vector, char_vector->capacity == 0 ? 2 : char_vector->capacity * 2);

    char_vector->buff[char_vector->size++] = c;
}

static void push_str(char_vector_t* char_vector, const char* str)
{
    for (int i = 0; str[i] != '\0'; ++i)
        push_char(char_vector, str[i]);
}




static const char* typeoperateur_to_str(typeoperateur type)
{
    // PLUS, MOINS, FOIS, DIV, PUIS 
    switch (type)
    {
        case PLUS:
            return "PLUS";
        case MOINS:
            return "MOINS";
        case FOIS:
            return "FOIS";
        case DIV:
            return "DIV";
        case PUIS:
            return "PUIS";
        default:
            // should never happen
            assert(false && "not implemented");
    }
    return NULL;
}
static const char* typefonction_to_str(typefonction type)
{
    switch (type)
    {
        case ABS:
            return "ABS";
        case SIN:
            return "SIN";
        case SQRT:
            return "SQRT";
        case LOG:
            return "LOG";
        case COS:
            return "COS";
        case TAN:
            return "TAN";
        case EXP:
            return "EXP";
        case ENTIER:
            return "ENTIER";
        case VAL_NEG:
            return "VAL_NEG";
        case SINC:
            return "SINC";
        default:
            // should never happen
            assert(false &&  "not implemented");
    }
    return NULL;
}

// this is used to convert a statically allocated const char* into a dynamic one, that can be freed
// must be freed
static char* str_copy(const char* str)
{
    size_t str_size = strlen(str);
    char* buff = malloc(str_size + 1);
    memcpy(buff, str, str_size + 1);
    return buff;
}

// can return null if the type does not come with a value
// must be freed
static char* get_str_value_for_typetoken(typetoken type, typevaleur valeur)
{
    switch (type)
    {
        case OPERATEUR:
            return str_copy(typeoperateur_to_str(valeur.operateur));
        case FONCTION:
            return str_copy(typefonction_to_str(valeur.fonction));
        case REEL:

            int required_size = snprintf(NULL, 0, "%f", valeur.reel) + 1;
            char* buff = malloc(required_size);
            snprintf(buff, required_size, "%f", valeur.reel);
            return buff;
    }
    return NULL;
}

static const char* typetoken_to_str(typetoken type)
{
    switch (type)
    {
    case REEL:
        return "REEL";
    case OPERATEUR:
        return "OPERATEUR";
    case FONCTION:
        return "FONCTION";
    case FIN:
        return "FIN";
    case PAR_OUV:
        return "PAR_OUV";
    case PAR_FERM:
        return "PAR_FERM";
    case VARIABLE:
        return "VARIABLE";
    case ABSOLU:
        return "ABSOLU";
    default:
        // should never happen
        assert(false && "not implemented");
    }
    return NULL;
}

// must be freed
static char* typejeton_to_string(typejeton token)
{
    char_vector_t char_vector = {0};
    push_str(&char_vector, typetoken_to_str(token.lexem));

    char* value = get_str_value_for_typetoken(token.lexem, token.valeur);
    if (value)
    {
        push_str(&char_vector, " : ");
        push_str(&char_vector, value);
        free(value);
    }
    push_char(&char_vector, '\0');
    return char_vector.buff;
}




char * lexical_tokens_to_str(const lexical_tokens_vector_t* tokens)
{
    if (!tokens || tokens->size == 0) return "";

    char_vector_t char_vector = {0};

    for (int i = 0; i < tokens-> size; ++i)
    {
        typejeton current_token = tokens->tokens[i];
        char* typejeton_str = typejeton_to_string(current_token);
        push_str(&char_vector, typejeton_str);
        free(typejeton_str);
        push_char(&char_vector, '\n');
    }
    push_char(&char_vector, '\0');

    return char_vector.buff;
}
