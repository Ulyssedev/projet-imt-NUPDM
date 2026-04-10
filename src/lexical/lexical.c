#include "lexical.h"
#include "lexical_vector.h"
#include <stdbool.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

// idea: indicate location of error with ^
// TODO: use size_t when possible
// TODO: tests
// TODO: avoid using strlen for for loop


typedef struct 
{
    char character;
    typejeton token;
} char_token_pair_t;
static char_token_pair_t special_character_token_pairs[] = 
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

typedef struct 
{
    const char* name;
    typejeton token;
} function_name_token_pair_t;
static function_name_token_pair_t function_name_token_pairs[] =
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


static bool is_alpha(char c)
{
    return 65 <= c <= 90;
}

static bool is_numerical(char c)
{
    return 48 <= c <= 57;
}

static bool is_alpha_numerical(char c)
{
    return is_alpha(c) || is_numerical(c);
}

static bool is_valid_function_char(char c)
{
    return is_alpha_numerical(c) || c == '_';
}

// compare a null terminated string (without including the null terminator), with a sequence of char of known size
static bool are_str_and_char_buff_equal(const char* str, const char* buff, int buff_size)
{
    if (strlen(str) != buff_size) return false;
    return memcmp(str, buff, buff_size) == 0;
}

// must be freed
static char* char_buff_to_null_terminated_string(const char* buff_start, int buff_size)
{
    char* null_terminated_string = malloc(buff_size + 1);
    memcpy(null_terminated_string, buff_start, buff_size);
    null_terminated_string[buff_size] = '\0';

    return null_terminated_string;
}

//  returns 0 if error
static int parse_reel_get_end(int begin_index, const char* expression, lexical_error_t* error)
{
    bool seen_dot = false;
    int end = begin_index + 1; // we have the guarrantee that first character is a valid reel char, see lexical_parse_tokensv2, so skip the first char
    for (;;++end)
    {
        char current_character = expression[end];
        bool is_dot = expression[end] == '.';
        bool first_dot = (is_dot && !seen_dot);
        if (is_numerical(current_character) || first_dot)
        {
            if (first_dot) seen_dot = true;
            continue;
        }

        // not a valid reel char, try to detect if it's an error, or the end of the token

        if (is_valid_function_char(current_character))
        {
            error->type = MALFORMED_REEL;
            sprintf_s(error->message, ERROR_MESSAGE_SIZE, 
                "Malfomed reel : at : %i, a reel can only contain digits or one . , got : %c instead", end, current_character);
            return 0;
        }

        if (is_dot)
        {
            error->type = MALFORMED_FUNCTION;
            sprintf_s(error->message, ERROR_MESSAGE_SIZE, "Malfomed reel : at : %i, a reel can only contain a single .", end);
            return 0;
        }

        return end;
    }
}

// returns lexem_size == 0 if error
typedef struct 
{
    typejeton token;
    size_t lexem_size;
} typejeton_and_size_t;
static typejeton_and_size_t parse_reel(int begin_index, const char* expression, lexical_error_t* error)
{
    typejeton_and_size_t error_rv = {0};

    int end = parse_reel_get_end(begin_index, expression, error);
    if (!end)
        return error_rv;

    const char* reel_begin = expression + begin_index;
    int reel_size = end - begin_index;
    char* null_terminated_reel_str = char_buff_to_null_terminated_string(reel_begin, reel_size);

    float parsed_float = strtof(null_terminated_reel_str, NULL);

    typejeton_and_size_t rv = {.lexem_size = reel_size, .token = {.lexem = REEL, .valeur = parsed_float}};

    return rv;
}

static bool is_space_or_newline(char c)
{
    return c == ' ' || c == '\n';
}

typedef enum
{
    DETECT_END_ERROR,
    DETECT_END_END,
    DETECT_END_NOTEND
} detect_end_result_t;

// weird design, TODO: follow same structure as parse_reel
static detect_end_result_t parse_function_detect_end(int possible_end_index, const char* expression, lexical_error_t* error)
{
    if (expression[possible_end_index] == '(')
        return DETECT_END_END;

    // if it's a space or a newline, then it's really the end of a function only if we have a succession of spaces/newlines that ends with a (
    if (is_space_or_newline(expression[possible_end_index]))
    {
        int j = possible_end_index + 1;
        while (is_space_or_newline(expression[j]))
            ++j;

        // expression[j] is now the character at the end of the succession of spaces/newlines
        if (expression[j] == '(')
            return DETECT_END_END;

        error->type = MALFORMED_FUNCTION;
        error->at_index = j;
        sprintf_s(error->message, ERROR_MESSAGE_SIZE, 
            "Malformed function : expected ( at : %i, got : %c instead, a function name must be followed by a (", 
        j, expression[j]);

        return DETECT_END_ERROR;
    }

    return DETECT_END_NOTEND;
}

static typejeton_and_size_t parse_function(int begin_index, const char* expression, lexical_error_t* error)
{
    typejeton_and_size_t error_rv = {0};

    int function_name_end = begin_index;
    for (int i = begin_index;; ++i)
    {
        function_name_end = i;

        detect_end_result_t status = parse_function_detect_end(i, expression, error);
        if (status == DETECT_END_END)
            break;
        if (status == DETECT_END_ERROR)
            return error_rv;

        // condition is also met when current character is \0
        if (!is_valid_function_char(expression[i]))
        {
            error->at_index = i;
            error->type = MALFORMED_FUNCTION;
            sprintf_s(error->message, ERROR_MESSAGE_SIZE, 
                "Malformed function : expected ( at %i, got %c instead, a function name must only contain alphanumerical characters or underscores and cannot contain spaces",
            i, expression[i]);
            return error_rv;
        }
    }

    int function_name_size = function_name_end - begin_index;
    const char* function_name_begin = expression + begin_index;

    for (int i = 0; i < sizeof(function_name_token_pairs) / sizeof(function_name_token_pair_t); ++i)
    {
        function_name_token_pair_t pair = function_name_token_pairs[i];
        if (are_str_and_char_buff_equal(pair.name, function_name_begin, function_name_size))
        {
            typejeton_and_size_t rv = {.lexem_size = function_name_size, .token = pair.token};
            return rv;
        }
    }


    error->at_index = begin_index;
    error->type = UNKNOWN_FUNCTION;

    char* null_terminated_function_name = char_buff_to_null_terminated_string(function_name_begin, function_name_size);
    sprintf_s(error->message, ERROR_MESSAGE_SIZE, "Uknown function : %s, at index : %i,  recognized functions : %s", null_terminated_function_name, begin_index, "TODO");
    free(null_terminated_function_name);

    return error_rv;
}

static typejeton_and_size_t parse_special_char_token(int begin_index, const char* expression, lexical_error_t* error)
{

    char current_character = expression[begin_index];
    for (int i = 0; i < sizeof(special_character_token_pairs) / sizeof(char_token_pair_t); ++i)
    {
        char_token_pair_t pair = special_character_token_pairs[i];
        if (pair.character == current_character)
        {
            typejeton_and_size_t rv = {.lexem_size = 1, .token = pair.token};
            return rv;
        }
    }

    typejeton_and_size_t error_rv = {0};
    error->at_index = begin_index;
    error->type = UNEXPECTED_CHARACTER;
    sprintf_s(error->message, ERROR_MESSAGE_SIZE, "Unexpected character : %c, at index : %i", current_character, begin_index);
    return error_rv;
}


// returns lexem size or 0 if error
static size_t parse
(
    lexical_tokens_vector_t* tokens, const char* expression, lexical_error_t* error, 
    int i,
    typejeton_and_size_t(*parser)(int begin_index, const char* expression, lexical_error_t* error)
)
{
    typejeton_and_size_t token = parser(i, expression, error);

    if (token.lexem_size)
        lexical_tokens_vector_push_back(tokens, &token.token);

    return token.lexem_size;
}

// TODO: remove newlines and spaces
lexical_tokens_vector_t lexical_parse_tokens(const char* expression, lexical_error_t* error)
{
    lexical_tokens_vector_t error_lexical_tokens = {0};
    lexical_tokens_vector_t tokens = {0};

    for (size_t i = 0; i < strlen(expression);)
    {
        char current_character = expression[i];
        if (is_space_or_newline(current_character)) continue;

        if (is_numerical(current_character) || current_character == '.')
        {
            size_t lexem_size = parse(&tokens, i, expression, error, parse_reel);
            if (!lexem_size) return error_lexical_tokens;
            i+= lexem_size;
            continue;
        }

        // no risk of overflow with expression[i+1] because worst case scenario it will be \0
        if (current_character == 'x' && !is_valid_function_char(expression[i+1]) && expression[i+1] != '(')
        {
            typejeton token = {.lexem = VARIABLE};
            lexical_tokens_vector_push_back(&tokens, &token);
            i+=1;
            continue;
        }

        if (is_valid_function_char(current_character))
        {
            size_t lexem_size = parse(&tokens, i, expression, error, parse_function);
            if (!lexem_size) return error_lexical_tokens;
            i+= lexem_size;
            continue;
        }

        size_t lexem_size = parse(&tokens, i, expression, error, parse_special_char_token);
        if (!lexem_size) return error_lexical_tokens;
        i+= lexem_size;
    }

    return tokens;
}