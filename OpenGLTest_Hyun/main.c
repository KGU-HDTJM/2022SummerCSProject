#include <GL/glut.h>
#include <GL/GL.h>
#include <GL/GLU.h>

#define W_W 300
#define W_H 300

int Lup = 0;
int Rup = 0;

void drawXAxis(void) 
{
	glColor3f(1, 1, 1);
	glBegin(GL_LINES);
	glVertex3f(0, 0, 0);
	glVertex3f(0.2, 0, 0);
	glVertex3f(0.2, 0, 0);
	glVertex3f(0.14, 0.06, 0);
	glVertex3f(0.2, 0, 0);
	glVertex3f(0.14, -0.06, 0);
	glEnd();
}

void drawAxis(void) 
{
	glColor3f(1, 1, 1);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	drawXAxis();

	glPushMatrix();
	glRotatef(90, 0, 0, 1);
	drawXAxis();
	glPopMatrix();

	glPushMatrix();
	glRotatef(-90, 0, 1, 0);
	drawXAxis();
	glPopMatrix();

}

void drawbody(void) 
{
	glPushMatrix();
	glScalef(2, 4, 1);
	glutWireCube(0.25);
	glPopMatrix();

}
void drawUpperLArm(void) 
{
	glPushMatrix();
	glTranslatef(0.25, 0.2, 0);
	glRotatef(30, 0, 0, 1);
	glTranslatef(0.25, 0, 0);

	glScalef(2, 0.8, 0.8);
	glutWireCube(0.25);
	glPopMatrix();
}

void drawDownLArm() 
{
	glPushMatrix();
	glRotated(90, 0, 0, 1);
	glTranslatef(0, -0.35, 0);
	glScaled(2, 0.8, 0.8);
	glPushMatrix();
	glutWireCube(0.25);
	glPopMatrix();

}

void drawUpperRArm() 
{
	glPushMatrix();
	glTranslatef(-0.25, 0.2, 0);
	glRotatef(-30, 0, 0, 1);
	glTranslatef(-0.25, 0, 0);

	glScalef(2, 0.8, 0.8);
	glutWireCube(0.25);
	glPopMatrix();
}

void drawDownRArm(void)
{
	glPushMatrix();
	glRotated(90, 0, 0, 1);
	glTranslatef(0, 0.35, 0);
	glScaled(2, 0.8, 0.8);
	glPushMatrix();
	glutWireCube(0.25);
	glPopMatrix();

}

void keyboard(unsigned char key, int x, int y) 
{
	switch (key)
	{
	case 'l':
		Lup = ~Lup;
		break;
	case 'r':
		Rup = ~Rup;
		break;

	}

	glutPostRedisplay();
}

void display(void)
{
	glClear(GL_COLOR_BUFFER_BIT);
	drawAxis();

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	drawbody();
	glPushMatrix();

	if (Lup) {
		drawUpperLArm();
	}
	else {
		drawDownLArm();
	}

	if (Rup)
	{
		drawUpperRArm();
	}
	else
	{
		drawDownRArm();
	}


	/*glPushMatrix();
		glTranslated(0.3,0,0);
		glRotatef(30, 0, 0, 1);
		glScalef(2, 1, 1);
		glutWireCube(0.2);*/
	glFlush();

}

void reshape(int width, int height) 
{
	glViewport(0, 0, width, height);
	GLfloat factor_w = (GLfloat)width / (GLfloat)W_W;
	GLfloat factor_h = (GLfloat)height / (GLfloat)W_H;

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(-1 * factor_w, 1 * factor_w, -1 * factor_h, 1 * factor_h, -2, 2);
	gluLookAt(0.5, 0.5, 1, 0, 0, 0, 0, 1, 0);
}


int main(int argc, char** argv) 
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGB);
	glutInitWindowSize(W_W, W_H);
	glutCreateWindow("window");
	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glutKeyboardFunc(keyboard);

	glutMainLoop();
	return 0;

}
