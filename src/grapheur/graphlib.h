// Fonctions utilitaires de dessin pour le module grapheur
// Ce fichier expose des helpers pour :
//  - initialiser la fenêtre OpenGL/GLUT
//  - appliquer la projection logique (world -> glOrtho)
//  - dessiner axes, grille, lignes, points et textes en coordonnées
//    world ou pixel

#ifndef GRAPHLIB_H
#define GRAPHLIB_H

#include "./utils/global.h"
#include "./utils/ndc.h"
#include "./utils/pixels.h"
#include "utils/world.h"
#include <GL/freeglut.h>
#include <GL/gl.h>
#include <GL/glut.h>
#include <math.h>
#include <stdbool.h>
#include <stdio.h>

/** Point 2D en coordonnées world. */
typedef struct {
  float x;
  float y;
} Point;

/**
 * Initialise une fenêtre GLUT et le contexte OpenGL.
 * @param argc pointeur vers argc du programme (transmis à GLUT)
 * @param argv argv du programme (transmis à GLUT)
 * @param w largeur de la fenêtre en pixels
 * @param h hauteur de la fenêtre en pixels
 * @param title titre de la fenêtre
 */
void graph_init_window(int *argc, char **argv, int w, int h, const char *title);

/** Définit la couleur de fond utilisée par `glClear(GL_COLOR_BUFFER_BIT)`. */
void graph_set_background(float r, float g, float b);

/**
 * Applique la vue logique courante (utilise `glOrtho`) et met la matrice
 * modelview à l'identité. À appeler avant les appels de dessin qui utilisent
 * des coordonnées world.
 */
void graph_apply_view(void);

/** Dessine les axes X et Y sur la vue logique actuelle. */
void graph_draw_axes(void);

/**
 * Dessine une grille régulière sur la vue logique.
 * @param x_step espacement entre lignes verticales
 * @param y_step espacement entre lignes horizontales
 */
void graph_draw_grid(float x_step, float y_step);

/**
 * Trace un nuage de points (scatter plot).
 * @param pts tableau de `Point`
 * @param n nombre de points
 * @param r,g,b composantes de couleur (0..1)
 * @param size taille du point en pixels
 */
void graph_plot_points(const Point *pts, int n, float r, float g, float b,
                       float size);

/**
 * Trace une polyligne connectée traversant les points fournis.
 * @param pts tableau de `Point`
 * @param n nombre de points
 * @param r,g,b composantes de couleur (0..1)
 * @param width épaisseur visuelle de la ligne en pixels
 */
void graph_plot_lines(const Point *pts, int n, float r, float g, float b,
                      float width);

/** Gère le redimensionnement de la fenêtre : met à jour le viewport et la
 * taille interne cache des pixels. Peut être passée à `glutReshapeFunc`.
 */
void graph_reshape(int w, int h);

/**
 * Dessine une chaîne de caractères bitmap en coordonnées fenêtre (pixels).
 * Origine en bas-gauche pour `x,y`.
 * @param text chaîne terminée par \0
 * @param x abscisse en pixels (depuis la gauche)
 * @param y ordonnée en pixels (depuis le bas)
 */
void graph_draw_text(const char *text, int x, int y);

/**
 * Dessine une chaîne de caractères bitmap en conservant la couleur OpenGL
 * courante.
 */
void graph_draw_text_current_color(const char *text, int x, int y);

/** Dessine du texte ancré près du coin supérieur gauche (padding fixe). */
void graph_draw_text_top_left(const char *text);
void graph_draw_text_top_right(const char *text);

/** Dessine les petites graduations sur les axes (lignes mineures). */
void graph_draw_grid_min_lines(float x_step, float y_step);

/** Dessine les valeurs numériques (étiquettes) le long des axes. */
void graph_draw_numbers(float x_step, float y_step);

/** Indique les coordonnées (coin supérieur droit). */
void graph_draw_coords_top_right(float x, float y);

/** Trace des lignes rouges reliant les axes au point de coordonnées donné. */
void graph_draw_coords_red_lines(float x, float y);

#endif // GRAPHLIB_H
