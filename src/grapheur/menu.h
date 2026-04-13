#ifndef MENU_H
#define MENU_H

#include "../evaluateur/eval.h"
#include "../dialogueur/pipeline.h"
#include "graphlib.h"
#include "utils/global.h"
#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

// Limites du menu : nombre max de fonctions et longueur max d'une chaîne
#define MENU_MAX_FUNCS 32
#define MENU_MAX_FUNC_LEN 256

/* Taille du buffer pour l'édition des bornes (min max) */
#define MENU_BOUNDS_INPUT_LEN 64

extern char funcs[MENU_MAX_FUNCS][MENU_MAX_FUNC_LEN];
extern int func_count;
extern int menu_visible;
extern int menu_selected;
extern int menu_editing;
extern int menu_adding;
extern char menu_input[MENU_MAX_FUNC_LEN];
extern int menu_input_pos;
/* Mode d'édition des bornes de la vue (0=none, 1=edit X, 2=edit Y) */
extern int menu_edit_bounds_mode;
extern char menu_bounds_input[MENU_BOUNDS_INPUT_LEN];
extern int menu_bounds_input_pos;

void mouse_motion(int x, int y);
void mouse_button(int button, int state, int x, int y);

// Gestion du cycle de vie du menu (initialisation / libération)
void menu_init(void);
void menu_shutdown(void);

// Accès aux fonctions enregistrées dans le menu
int menu_get_count(void);
void menu_get_function(int idx, char *out, int out_size);

// Ajout / modification / suppression d'une fonction. Les chaînes sont
// copiées dans le stockage interne du menu.
int menu_add_function(const char *func_str);
int menu_edit_function(int idx, const char *func_str);
int menu_remove_function(int idx);

// Indicateur de mise à jour : permet à l'interface principale de savoir
// si la liste des fonctions a changé depuis le dernier affichage.
int menu_is_updated(void);
void menu_clear_updated_flag(void);

/* Helpers d'affichage (GLUT) pour le menu visuel */
void menu_toggle_visible(void);
void menu_set_visible(int visible);
void menu_draw_overlay(void);
void menu_draw_button(void);
int menu_handle_mouse(int button, int state, int x, int y);
int menu_handle_key(unsigned char key, int x, int y);
int menu_handle_special(int key, int x, int y);

// Récupère l'arbre (AST) mis en cache pour la fonction d'index `idx`.
// Retourne NULL si la fonction n'a pas de parse valide. Le pointeur renvoyé
// est détenu par le module `menu` et NE DOIT PAS être libéré par l'appelant.
Arbre menu_get_cached_arbre(int idx);

#endif // MENU_H
