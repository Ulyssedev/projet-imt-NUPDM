#include "../common/jeton.h"
#include <stddef.h>


typedef struct
{
    enum type_t
    {
        UNKNOWN_FUNCTION,
        INVALID_CHARACTER
    };
    char* message;
} lexical_error_t;


typedef struct
{
    size_t size;
    typejeton* tokens;
} lexical_tokens_t;

// puissance : ^
//sin(x*abs(x))+2

// if error_t* is not nullptr error_t::message must be freed after use
// expression must be null terminated ascii
// lexical_tokens_t::tokens must be freed if not null
lexical_tokens_t lexical_parse_tokens(const char* expression, lexical_error_t* error/* optional */);