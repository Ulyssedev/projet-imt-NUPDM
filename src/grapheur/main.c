#include <GL/glut.h>

void display() {
    glClear(GL_COLOR_BUFFER_BIT); // Clear the screen

    glBegin(GL_TRIANGLES); // Start drawing a triangle
        glColor3f(1.0, 0.0, 0.0); // Red
        glVertex2f(-0.5, -0.5);
        glColor3f(0.0, 1.0, 0.0); // Green
        glVertex2f(0.5, -0.5);
        glColor3f(0.0, 0.0, 1.0); // Blue
        glVertex2f(0.0, 0.5);
    glEnd();

    glFlush(); // Render now
}

// Main function
int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
    glutInitWindowSize(500, 500);
    glutInitWindowPosition(100, 100);
    glutCreateWindow("OpenGL Test");

    glClearColor(0.0, 0.0, 0.0, 1.0);

    glutDisplayFunc(display);

    glutMainLoop();

    return 0;
}