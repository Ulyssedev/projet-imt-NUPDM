#include <stdlib.h>
#include <stdio.h>

#include "lexical.h"

// int main()
// {
//     // test lexical_tokens_to_str
//     // typejeton test_tokens[] =
//     // {
//     //     {.lexem = OPERATEUR, .valeur.operateur = PLUS},
//     //     {.lexem = REEL, .valeur.reel = 3.4},
//     //     {.lexem = FONCTION, .valeur.fonction = SIN},
//     //     {.lexem = VARIABLE}
//     // };

//     // lexical_tokens_vector_t tokens_vector = 
//     // {
//     //     .size = sizeof(test_tokens) / sizeof(typejeton), 
//     //     .capacity = sizeof(test_tokens) / sizeof(typejeton), 
//     //     .tokens = test_tokens
//     // };
//     // char* to_str = lexical_tokens_to_str(&tokens_vector);
//     // printf(to_str);
//     // free(to_str);


//     const char* test_expressions[] = 
//     {
//         "sin(x^2 + 3 - 6)",
//         "foijdrg+(x^2 + 3 - 6)",
//         "cos          \n( sin ( x))",
//         "cos ",
//         "cos",
//         "cos cos",
//         "3 4 5",
//         "3f",
//         ".",
//         "exp(.4)"
//         " \nexp(3.)",
//         "sinc(   \n abs(\nx))",
//         "cos+sin",
//         "x()",
//         "x  ()",
//         "3.5.6"
//     };

//     for (size_t i = 0; i < sizeof(test_expressions)/sizeof(const char*); ++i)
//     {
//         const char* expression = test_expressions[i];
//         lexical_error_t error = {0};
//         lexical_tokens_vector_t tokens = lexical_parse_tokens(expression, &error);

//         if (error.type)
//             printf("error for \"%s\":\n%s\n\n", expression, error.message);
//         else
//         {
//             char* to_str = lexical_tokens_to_str(&tokens);
//             printf("success for \"%s\":\n%s\n", expression, to_str);
//             free(to_str);
//         }
//         lexical_tokens_vector_free(&tokens);
//     }
// }