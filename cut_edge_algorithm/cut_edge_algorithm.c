#define _CRT_SECURE_NO_WARNINGS

#include <GL/freeglut.h>
#include <stdio.h>
#include <math.h>
#include "HDTJMType.h"

#define LEFT -1.0f
#define RIGHT 1.0f
#define TOP 1.0f
#define BOTTOM -1.0f

typedef struct Point {
	float x;
	float y;
}Point;

typedef struct Line {
	Point p1;
	Point p2;

}Line;

Line square_s[4] = {
	{{LEFT,TOP},{RIGHT,TOP}},
	{{RIGHT,TOP},{RIGHT,BOTTOM}},
	{{RIGHT,BOTTOM},{LEFT,BOTTOM}},
	{{LEFT,BOTTOM},{LEFT,TOP}}
};

#define W_H 500
#define W_W 500



int wire = 1;

float tirPosx;
float tirPosy;
float size = 0.5;
//Object obj1;

void DrawSq(void)
{
	float vertex[4][3] = {
		{RIGHT,TOP,1.0f},
		{RIGHT,BOTTOM,1.0f},
		{LEFT,BOTTOM,1.0f},
		{LEFT,TOP,1.0f}
	};

	GLint Face[4] = { 0,1,2,3 };

	glVertexPointer(3, GL_FLOAT, 0, vertex);
	glDrawElements(GL_QUADS, 4, GL_UNSIGNED_INT, Face);
}

float CrossProduct(Point a, Point b)
{
	return a.x * b.y - a.y * b.x;
}

float Direction(Point a, Point b, Point c)
{
	Point ca = { a.x - c.x, a.y - c.y };
	Point cb = { b.x - c.x, b.y - c.y };
	float value;

	value = CrossProduct(ca, cb);
	return value == 0 ? 0 : (value > 0 ? 1 : -1);
}

boolean_t OnSegment(Point a, Point b, Point c)
{
	if (c.x >= min(a.x, b.x) && c.x <= max(a.x, b.x) && c.y >= min(a.y, b.y) && c.y <= max(a.y, b.y)) 
		return True;
	return False;
}
boolean_t IsIntersected(Line l1, Line l2) 
{
	int d1, d2, d3, d4;
	d1 = Direction(l2.p1, l2.p2, l1.p1);
	d2 = Direction(l2.p1, l2.p2, l1.p2);
	d3 = Direction(l1.p1, l1.p2, l2.p1);
	d4 = Direction(l1.p1, l1.p2, l2.p2);
	if (d1 * d2 < 0 && d3 * d4 < 0) return True;
	else if (d1 == 0 && OnSegment(l2.p1, l2.p2, l1.p1)) return True;
	else if (d2 == 0 && OnSegment(l2.p1, l2.p2, l1.p2)) return True;
	else if (d3 == 0 && OnSegment(l1.p1, l1.p2, l2.p1)) return True;
	else if (d4 == 0 && OnSegment(l1.p1, l1.p2, l2.p2)) return True;
	return False;
}

void ReviseVertex(float vertex[3][3])
{
	for (int i = 0; i < 3; i++)
	{
		Line triSide = {
			{vertex[i][0],vertex[i][1]},
			{vertex[(i + 1) % 3][0],vertex[(i + 1) % 3][1]}
		};
		//printf("%.1f %.1f\n", triSide.p1.x, triSide.p1.y);
		for (int j = 0; j < 4; j++)
		{
			Line sqrSide = square_s[j];
			float d = (triSide.p1.x - triSide.p2.x) * (sqrSide.p1.y - sqrSide.p2.y)
				- (triSide.p1.y - triSide.p2.y) * (sqrSide.p1.x - sqrSide.p2.x);
			if (IsIntersected(triSide, sqrSide) && d != 0.0f)
			{
				Point point = {
					((triSide.p1.x * triSide.p2.y - triSide.p1.y * triSide.p2.x) * (sqrSide.p1.x - sqrSide.p2.x)
					- (triSide.p1.x - triSide.p2.x) * (sqrSide.p1.x * sqrSide.p2.y - sqrSide.p1.y * sqrSide.p2.x)) / d,
					((triSide.p1.x * triSide.p2.y - triSide.p1.y * triSide.p2.x) * (sqrSide.p1.y - sqrSide.p2.y)
					- (triSide.p1.y - triSide.p2.y) * (sqrSide.p1.x * sqrSide.p2.y - sqrSide.p1.y * sqrSide.p2.x)) / d
				};

				printf("%.1f %.1f\n", point.x, point.y);
			}
		}
	}
}
void DrawTri(float x, float y, float size, float angle)
{
	float vertex[3][3] = {
		{0.0f * size + x,1.0f * size + y,1.0f},
		{0.0f * size + x,-1.0f * size + y,1.0f},
		{1.0f * size + x,0.0f * size + y,1.0f},
	};

	GLint Face[3] = { 0,1,2 };
	glVertexPointer(3, GL_FLOAT, 0, vertex);
	ReviseVertex(vertex);
	glRotatef(angle, 0.0, 0.0, 1.0);


	glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_INT, Face);
}

void Display(void)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	gluLookAt(0, 0, 1, 0.0, 0.0, 0.0, 0, 1, 0);
	if (wire)glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glPushMatrix();

	DrawSq();
	glPopMatrix();
	glPushMatrix();
	DrawTri(tirPosx, tirPosy, size, 0);
	glPopMatrix();
	glFlush();
}
void Reshape(int width, int height)
{
	glViewport(0, 0, width, height);
	GLfloat factor_w = (GLfloat)width / (GLfloat)W_W;
	GLfloat factor_h = (GLfloat)height / (GLfloat)W_H;

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(-1 * factor_w, 1 * factor_w, -1 * factor_h, 1 * factor_h, -10, 10);

}

void keyboard(unsigned char key, int x, int y)
{
	switch (key)
	{
	case 'w':
		tirPosy += 0.1;
		break;
	case 'a':
		tirPosx -= 0.1;
		break;
	case 's':
		tirPosy -= 0.1;
		break;
	case 'd':
		tirPosx += 0.1;
		break;

	default:
		break;
	}


	glutPostRedisplay();
}




int main(int argc, char** argv)
{

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DEPTH | GLUT_SINGLE);
	glutInitWindowSize(1000, 1000);
	glutInitWindowPosition(300, 200);
	glClearColor(0.0, 0.0, 0.0, 0.0);//rgba (a: 투명도)

	glutCreateWindow("Lighting");
	glEnableClientState(GL_VERTEX_ARRAY);
	glutDisplayFunc(Display);
	glutReshapeFunc(Reshape);
	glutKeyboardFunc(keyboard);
	glutMainLoop();

	return 0;

}