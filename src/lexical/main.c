#include <stdlib.h>
#include <stdio.h>

#include "lexical.h"

size_t test_method()
{
    return 1;
}

static const char* test_expressions[] = 
{
    "sin(x^2 + 3    - 6)"
};

int main()
{
    typejeton tokens[] =
    {
        {.lexem = OPERATEUR, .valeur.operateur = PLUS},
        {.lexem = REEL, .valeur.reel = 3.4},
        {.lexem = FONCTION, .valeur.fonction = SIN},
        {.lexem = VARIABLE}
    };

    lexical_tokens_vector_t tokens_vector = {.size = sizeof(tokens), .capacity = sizeof(tokens), .tokens = tokens};

    printf(lexical_tokens_to_str(&tokens_vector));
}