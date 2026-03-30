#include "../common/jeton.h"
#include <stddef.h>

//defines the maximum size of a multi_char token like a function name or a number, including null terminator
// a value of 256 means a function name / number can't be longer than 255 characters
#define MAX_MULTI_CHAR_SIZE 256
#define ERROR_MESSAGE_SIZE 512

typedef enum
{
    UNKNOWN_FUNCTION,
    INVALID_CHARACTER,
    TOKEN_TOO_LONG
} lexical_error_type_t;
typedef struct
{
    lexical_error_type_t type;
    int at_index;
    char message[ERROR_MESSAGE_SIZE]; // null terminated
} lexical_error_t;


typedef struct
{
    size_t size;
    typejeton* tokens;
} lexical_tokens_t;

// puissance : ^
//sin(x*abs(x))+2

// in case of an error, lexical_tokens_t::tokens and lexical_tokens_t::size will be null
// expression must be null terminated ascii
// lexical_tokens_t::tokens must be freed if not null
lexical_tokens_t lexical_parse_tokens(const char* expression, lexical_error_t* error /* out optional, set to NULL if not used */);