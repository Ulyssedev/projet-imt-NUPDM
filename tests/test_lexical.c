#include "../src/lexical/lexical.h"
#include "../src/lexical/lexical_vector.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// ============================================================
//  Petit programme de test pour l'analyseur lexical
//  Compile (windows): gcc -o test lexical.c lexical_vector.c lexical_to_str.c
//  test.c Compile (Mac): clang -o test lexical.c lexical_vector.c
//  lexical_to_str.c test.c -I.. Execute : ./test
// ============================================================

static void tester(const char *expression) {
  printf("--------------------------------------------------\n");
  printf("Expression : \"%s\"\n", expression ? expression : "(NULL)");

  lexical_error_t erreur = {0};
  lexical_tokens_vector_t resultat = lexical_parse_tokens(expression, &erreur);

  if (resultat.size == 0 && (expression == NULL || strlen(expression) > 0)) {
    printf("ERREUR : %s\n", erreur.message);
  } else {
    char *str = lexical_tokens_to_str(&resultat);
    printf("Tokens (%zu) :\n%s", resultat.size, str);
    free(str);
    lexical_tokens_vector_free(&resultat);
  }
}

int main(void) {
  printf("========== CAS NORMAUX ==========\n\n");

  tester("x");
  tester("3.14");
  tester("(x+2)");
  tester("(x*3.5)");
  tester("sin(x)");
  tester("cos(x)");
  tester("sqrt(x)");
  tester("sin(x*abs(x))+2");
  tester("(x+(6*log(x+1)))");
  tester("sin(x^2 + 3 - 6)");

  printf("\n========== CAS D'ERREURS ==========\n\n");

  tester(NULL);                           // expression nulle
  tester("3.4.5");                        // deux points dans un réel
  tester("toto(x)");                      // fonction inconnue
  tester("COS(lexical_tokens_to_str'x)"); // majuscules non reconnues
  tester("@");                            // caractère invalide

  return 0;
}