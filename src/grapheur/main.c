#include <GL/glut.h>
#include <math.h>

void tracerLigneHorizontale(float y, float x_min, float x_max);
void tracerLigneVerticale(float x, float y_min, float y_max);


void display() {
    glClear(GL_COLOR_BUFFER_BIT); // Clear the screen

    tracerLigneHorizontale(0.0,-1.0,1.0);
    tracerLigneVerticale(0.0, -1.0, 1.0);

    glFlush(); // Render now
}

void tracerLigneHorizontale(float y, float x_min, float x_max) {
    glBegin(GL_LINES);
        glColor3f(1,1,1);
        glVertex2f(x_min, y); // Point de départ (tout à gauche)
        glVertex2f(x_max, y); // Point d'arrivée (tout à droite)
    glEnd();
}

void tracerLigneVerticale(float x, float y_min, float y_max) {
    glBegin(GL_LINES);
        glColor3f(1,1,1);
        glVertex2f(x, y_min); // Point de départ (tout en bas)
        glVertex2f(x, y_max); // Point d'arrivée (tout en haut)
    glEnd();
}

// Main function
int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
    glutInitWindowSize(1000, 1000);
    glutInitWindowPosition(100, 100);
    glutCreateWindow("Graphique");

    glClearColor(0.0, 0.0, 0.0, 1.0);

    glutDisplayFunc(display);

    glutMainLoop();

    return 0;
}

// gcc ./src/grapheur/main.c -o opengl_test -lGL -lGLU -lglut