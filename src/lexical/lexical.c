#include "lexical.h"

// Idée : indiquer la position de l'erreur avec ^
// TODO : tests
// TODO : renommer parse_function

// Utilisé pour convertir un const char* statique en chaîne dynamique,
// qui pourra ensuite être libérée.
// doit être libéré
static char *str_copy(const char *str) {
  size_t str_size = strlen(str);
  char *buff = malloc(str_size + 1);
  memcpy(buff, str, str_size + 1);
  return buff;
}

// Convertit par exemple ' ' en "SPACE" pour les messages d'erreur qui doivent
// afficher le caractère problématique.
// doit être libéré
static char *convert_non_displayable_char(char c) {
  switch (c) {
  case ' ':
    return str_copy("SPACE");
  case '\n':
    return str_copy("NEWLINE");
  case '\0':
    return str_copy("NULL");
  default:
    break;
  }
  char *rv = malloc(2);
  rv[0] = c;
  rv[1] = '\0';
  return rv;
}

static bool is_alpha(char c) {
  return (65 <= c && c <= 90) || (97 <= c && c <= 122);
}

static bool is_numerical(char c) { return 48 <= c && c <= 57; }

static bool is_alpha_numerical(char c) {
  return is_alpha(c) || is_numerical(c);
}

static bool is_valid_function_char(char c) {
  return is_alpha_numerical(c) || c == '_';
}

// Compare une chaîne terminée par \0 (sans compter le terminateur) avec une
// séquence de caractères de taille connue.
static bool are_str_and_char_buff_equal(const char *str, const char *buff,
                                        size_t buff_size) {
  if (strlen(str) != buff_size)
    return false;
  return memcmp(str, buff, buff_size) == 0;
}

// doit être libéré
static char *char_buff_to_null_terminated_string(const char *buff_start,
                                                 size_t buff_size) {
  char *null_terminated_string = malloc(buff_size + 1);
  memcpy(null_terminated_string, buff_start, buff_size);
  null_terminated_string[buff_size] = '\0';

  return null_terminated_string;
}

// Renvoie 0 en cas d'erreur.
static size_t parse_real_get_end(size_t begin_index, const char *expression,
                                 lexical_error_t *error) {
  bool seen_dot = false;
  size_t end =
      begin_index +
      1; // On a la garantie que le premier caractère est un caractère valide,
         // voir lexical_parse_tokensv2, donc skip le premier caractère
  for (;; ++end) {
    char current_character = expression[end];
    bool is_dot = expression[end] == '.';
    bool first_dot = (is_dot && !seen_dot);
    if (is_numerical(current_character) || first_dot) {
      if (first_dot)
        seen_dot = true;
      continue;
    }

    // Caractère de réel invalide : on essaie de savoir s'il s'agit d'une
    // erreur ou simplement de la fin du jeton.

    if (is_valid_function_char(current_character)) {
      error->type = MALFORMED_REAL;
      snprintf(error->message, ERROR_MESSAGE_SIZE,
               "Reel mal forme : a l'index %zu, un reel ne peut contenir que "
               "des chiffres ou un seul . , on a : %s"
               "\nUn nom de fonction ne peut pas commencer par un chiffre"

               ,
               end + 1, convert_non_displayable_char(current_character));
      return 0;
    }

    if (is_dot) {
      error->type = MALFORMED_REAL;
      snprintf(error->message, ERROR_MESSAGE_SIZE,
               "Reel mal forme : a l'index %zu, un reel ne peut contenir qu'un "
               "seul .",
               end + 1);
      return 0;
    }

    return end;
  }
}

// Renvoie lexem_size == 0 en cas d'erreur.
// typejeton::token est le jeton analyse a ajouter au vecteur de jetons.
// lexem_size doit etre le nombre de caracteres consommes dans l'expression.
// On en deduit que le prochain jeton commence a current_index + lexem_size.
typedef struct {
  typejeton token;
  size_t lexem_size;
} typejeton_and_size_t;
static typejeton_and_size_t
parse_real(size_t begin_index, const char *expression, lexical_error_t *error) {
  typejeton_and_size_t error_rv = {0};

  size_t end = parse_real_get_end(begin_index, expression, error);
  if (!end)
    return error_rv;

  const char *real_begin = expression + begin_index;
  size_t real_size = end - begin_index;
  char *null_terminated_real_str =
      char_buff_to_null_terminated_string(real_begin, real_size);

  float parsed_float = strtof(null_terminated_real_str, NULL);

  free(null_terminated_real_str);

  typejeton_and_size_t rv = {
      .lexem_size = real_size,
      .token = {.lexem = REEL, .valeur.reel = parsed_float}};

  return rv;
}

static bool is_space_or_newline(char c) { return c == ' ' || c == '\n'; }

static bool is_unary_sign_context(size_t begin_index, const char *expression) {
  if (begin_index == 0)
    return true;

  size_t previous_index = begin_index;
  while (previous_index > 0 &&
         is_space_or_newline(expression[previous_index - 1])) {
    --previous_index;
  }

  if (previous_index == 0)
    return true;

  char previous_character = expression[previous_index - 1];
  return previous_character == '(' || previous_character == '+' ||
         previous_character == '-' || previous_character == '*' ||
         previous_character == '/' || previous_character == '^' ||
         previous_character == '|';
}

static bool is_signed_real_start(size_t begin_index, const char *expression) {
  char current_character = expression[begin_index];
  if (current_character != '-' && current_character != '+')
    return false;

  char next_character = expression[begin_index + 1];
  if (!is_numerical(next_character) && next_character != '.')
    return false;

  return is_unary_sign_context(begin_index, expression);
}

typedef enum {
  DETECT_END_ERROR,
  DETECT_END_END,
  DETECT_END_NOTEND
} detect_end_result_t;

// Conception un peu bizarre, TODO : suivre la meme structure que parse_real.
static detect_end_result_t parse_function_detect_end(size_t possible_end_index,
                                                     const char *expression) {
  if (expression[possible_end_index] == '(')
    return DETECT_END_END;

  // S'il s'agit d'un espace ou d'un retour a la ligne, on ne peut conclure
  // a la fin d'une fonction que si la suite d'espaces/retours a la ligne se
  // termine par un '('.
  if (is_space_or_newline(expression[possible_end_index])) {
    size_t j = possible_end_index + 1;
    while (is_space_or_newline(expression[j]))
      ++j;

    // expression[j] est maintenant le caractere qui termine la suite
    // d'espaces/retours a la ligne.
    if (expression[j] == '(')
      return DETECT_END_END;

    return DETECT_END_ERROR;
  }

  return DETECT_END_NOTEND;
}

// Renvoie 0 si le nom de fonction est mal formé et place l'indice
// problématique dans *error_at.
static size_t get_function_name_end(size_t begin_index, const char *expression,
                                    size_t *error_at) {
  size_t function_name_end = begin_index + 1;
  for (;; ++function_name_end) {
    detect_end_result_t status =
        parse_function_detect_end(function_name_end, expression);
    if (status == DETECT_END_END)
      break;
    if (status == DETECT_END_ERROR ||
        !is_valid_function_char(expression[function_name_end])) {
      *error_at = function_name_end;
      return 0;
    }
  }
  return function_name_end;
}

static typejeton_and_size_t parse_function_or_variable(size_t begin_index,
                                                       const char *expression,
                                                       lexical_error_t *error) {
  typedef struct {
    const char *name;
    typejeton token;
  } function_name_token_pair_t;
  static const function_name_token_pair_t function_name_token_pairs[] = {
      {"abs", {.lexem = FONCTION, .valeur.fonction = ABS}},
      {"sin", {.lexem = FONCTION, .valeur.fonction = SIN}},
      {"sqrt", {.lexem = FONCTION, .valeur.fonction = SQRT}},
      {"log", {.lexem = FONCTION, .valeur.fonction = LOG}},
      {"cos", {.lexem = FONCTION, .valeur.fonction = COS}},
      {"tan", {.lexem = FONCTION, .valeur.fonction = TAN}},
      {"exp", {.lexem = FONCTION, .valeur.fonction = EXP}},
      {"entier", {.lexem = FONCTION, .valeur.fonction = ENTIER}},
      {"val_neg", {.lexem = FONCTION, .valeur.fonction = VAL_NEG}},
      {"sinc", {.lexem = FONCTION, .valeur.fonction = SINC}}};

  typejeton_and_size_t error_rv = {0};

  size_t error_at = 0;
  size_t function_name_end =
      get_function_name_end(begin_index, expression, &error_at);
  if (error_at) {
    // Malgré l'absence de '(', il peut encore s'agir d'une variable,
    // puisque 'x' est un caractère autorisé dans un nom.
    if (expression[begin_index] == 'x') {
      typejeton_and_size_t token_and_size = {.lexem_size = 1,
                                             .token = {.lexem = VARIABLE}};
      return token_and_size;
    }

    error->at_index = error_at;
    error->type = MALFORMED_FUNCTION;
    snprintf(error->message, ERROR_MESSAGE_SIZE,
             "Fonction mal formee : '(' attendu a l'index %zu, on a %s a la "
             "place ; un nom de fonction doit etre suivi de '('"
             ", ne contenir que des caracteres alphanumeriques ou des "
             "underscores et ne pas contenir d'espaces",
             error_at + 1, convert_non_displayable_char(expression[error_at]));
    return error_rv;
  }

  size_t function_name_size = function_name_end - begin_index;
  const char *function_name_begin = expression + begin_index;

  for (size_t i = 0; i < sizeof(function_name_token_pairs) /
                             sizeof(function_name_token_pair_t);
       ++i) {
    function_name_token_pair_t pair = function_name_token_pairs[i];
    if (are_str_and_char_buff_equal(pair.name, function_name_begin,
                                    function_name_size)) {
      typejeton_and_size_t rv = {.lexem_size = function_name_size,
                                 .token = pair.token};
      return rv;
    }
  }

  error->at_index = begin_index;
  error->type = UNKNOWN_FUNCTION;

  char *null_terminated_function_name = char_buff_to_null_terminated_string(
      function_name_begin, function_name_size);
  snprintf(error->message, ERROR_MESSAGE_SIZE,
           "Fonction inconnue : %s, caractere numero : %zu",
           null_terminated_function_name, begin_index + 1);
  free(null_terminated_function_name);

  return error_rv;
}

static typejeton_and_size_t parse_special_char_token(size_t begin_index,
                                                     const char *expression,
                                                     lexical_error_t *error) {
  typedef struct {
    char character;
    typejeton token;
  } char_token_pair_t;
  static const char_token_pair_t special_character_token_pairs[] = {
      {'+', {.lexem = OPERATEUR, .valeur.operateur = PLUS}},
      {'-', {.lexem = OPERATEUR, .valeur.operateur = MOINS}},
      {'*', {.lexem = OPERATEUR, .valeur.operateur = FOIS}},
      {'/', {.lexem = OPERATEUR, .valeur.operateur = DIV}},
      {'^', {.lexem = OPERATEUR, .valeur.operateur = PUIS}},
      {'(', {.lexem = PAR_OUV}},
      {')', {.lexem = PAR_FERM}},
      {'|', {.lexem = ABSOLU}}};

  char current_character = expression[begin_index];
  for (size_t i = 0;
       i < sizeof(special_character_token_pairs) / sizeof(char_token_pair_t);
       ++i) {
    char_token_pair_t pair = special_character_token_pairs[i];
    if (pair.character == current_character) {
      typejeton_and_size_t rv = {.lexem_size = 1, .token = pair.token};
      return rv;
    }
  }

  typejeton_and_size_t error_rv = {0};
  error->at_index = begin_index;
  error->type = UNEXPECTED_CHARACTER;
  snprintf(error->message, ERROR_MESSAGE_SIZE,
           "Caractere inattendu : %c, a l'index : %zu", current_character,
           begin_index + 1);
  return error_rv;
}

// Renvoie la taille du lexeme, ou 0 en cas d'erreur.
static size_t parse(lexical_tokens_vector_t *tokens, size_t i,
                    const char *expression, lexical_error_t *error,
                    typejeton_and_size_t (*parser)(size_t begin_index,
                                                   const char *expression,
                                                   lexical_error_t *error)) {
  typejeton_and_size_t token = parser(i, expression, error);

  if (token.lexem_size)
    lexical_tokens_vector_push_back(tokens, &token.token);
  else {
    // Le lexeur a rencontre une erreur, on libere la memoire avant de sortir.
    lexical_tokens_vector_free(tokens);
  }

  return token.lexem_size;
}

// TODO : supprimer les retours à la ligne et les espaces
lexical_tokens_vector_t lexical_parse_tokens(const char *expression,
                                             lexical_error_t *error) {
  lexical_tokens_vector_t error_lexical_tokens = {0};
  lexical_tokens_vector_t tokens = {0};

  if (!expression) {
    error->type = NULL_EXPRESSION;
    snprintf(error->message, ERROR_MESSAGE_SIZE, "l'expression est nulle");
    return error_lexical_tokens;
  }

  for (size_t i = 0;;) {
    char current_character = expression[i];
    if (current_character == '\0')
      break;

    if (is_space_or_newline(current_character)) {
      i += 1;
      continue;
    }

    if (is_numerical(current_character) || current_character == '.' ||
        is_signed_real_start(i, expression)) {
      size_t lexem_size = parse(&tokens, i, expression, error, parse_real);
      if (!lexem_size)
        return error_lexical_tokens;
      i += lexem_size;
      continue;
    }

    if (is_valid_function_char(current_character)) {
      size_t lexem_size =
          parse(&tokens, i, expression, error, parse_function_or_variable);
      if (!lexem_size)
        return error_lexical_tokens;
      i += lexem_size;
      continue;
    }

    size_t lexem_size =
        parse(&tokens, i, expression, error, parse_special_char_token);
    if (!lexem_size)
      return error_lexical_tokens;
    i += lexem_size;
  }

  if (tokens.size == 0) {
    error->type = NO_TOKENS;
    sprintf(error->message, "aucun jeton n'a ete trouve dans l'expression");
    return error_lexical_tokens;
  }

  return tokens;
}
