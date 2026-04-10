#include "../common/jeton.h"
#include "lexical_vector.h"
#include <stddef.h>

// BROKEN

//defines the maximum size of a multi_char token like a function name or a number, including null terminator
// a value of 256 means a function name / number can't be longer than 255 characters
#define MAX_MULTI_CHAR_SIZE 256
#define ERROR_MESSAGE_SIZE 512

typedef enum
{
    UNKNOWN_FUNCTION,
    MALFORMED_FUNCTION,
    MALFORMED_REEL,
    UNEXPECTED_CHARACTER,
    EMPTY_EXPRESSIION
} lexical_error_type_t;

typedef struct
{
    lexical_error_type_t type;
    int at_index;
    char message[ERROR_MESSAGE_SIZE]; // null terminated
} lexical_error_t;


//sin(x*abs(x))+2
// if error, lexical_tokens_vector_t::size is 0
// lexical_tokens_vector_t must be freed with lexical_tokens_vector_free
// expression must be null terminated ascii
lexical_tokens_vector_t lexical_parse_tokens(const char* expression, lexical_error_t* error);

// convert a lexical_tokens_vector_t into a detailed string about its tokens
// must be freed
char * lexical_tokens_to_str(lexical_tokens_vector_t* tokens);