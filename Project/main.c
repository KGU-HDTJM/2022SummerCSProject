#include <GL/freeglut.h>
#include <stdio.h>
#include <math.h>
#include "Object.h"

#define W_H 500
#define W_W 500
int bim = 1;

Object obj1;
GLfloat R = 0.7;
GLfloat camx = 0.5, camy = 0.5, camz = 0.5;
GLfloat hrzndegree = 0, vrtcldegree = 0;
void Display(void)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	camy = R * sin(vrtcldegree);
	camx = pow(R * R + camy * camy, (double)0.5) * cos(hrzndegree);
	camz = pow(R * R + camy * camy, (double)0.5) * sin(hrzndegree);
	gluLookAt(camx, camy, camz, 0, 0, 0, 0, 1, 0);
	DrawObject(obj1);
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

void init_light(void)
{
	glEnable(GL_LIGHTING);
	glShadeModel(GL_SMOOTH);
	glEnable(GL_DEPTH_TEST);

	glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER,GL_TRUE);

	GLfloat light_global_ambient[] = {0.3,0.3,0.3,1.0};
	GLfloat light_0_pos[] = { 2.0 , 2.0 , -2.0 , 1.0};

	GLfloat light_0_ambient[] = {1.0 , 1.0 , 1.0 , 1.0};
	GLfloat light_0_diffuse[] = {1.0 , 1.0 , 1.0 , 1.0};
	GLfloat light_0_specular[] = {1.0 ,1.0 , 1.0, 1.0};

	glLightModelfv(GL_LIGHT_MODEL_AMBIENT,light_global_ambient);
	glEnable(GL_LIGHT0);
	
	
	glLightfv(GL_LIGHT0, GL_AMBIENT, light_0_ambient);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, light_0_diffuse);
	glLightfv(GL_LIGHT0, GL_SPECULAR, light_0_specular);
	glLightfv(GL_LIGHT0, GL_POSITION, light_0_pos);


	

	glDisable(GL_COLOR_MATERIAL);
	GLfloat matrial_0_ambient[] = { 0.3,0.3,0.3,1.0};
	GLfloat matrial_0_diffuse[] = {0.8,0.0,0.3,1.0}; // 물체의 색상을 가장 직접적으로 결정
	GLfloat matrial_0_specular[] = {1.0 , 1.0 ,1.0 ,1.0};
	GLfloat matrial_0_shininess[] = {25.0};
	glMaterialfv(GL_FRONT,GL_AMBIENT, matrial_0_ambient);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, matrial_0_diffuse);
	glMaterialfv(GL_FRONT, GL_SPECULAR, matrial_0_specular);
	glMaterialfv(GL_FRONT, GL_SHININESS, matrial_0_shininess);

	//물체 색상 정의 & 지정
	


}
void keyboard(unsigned char key, int x, int y)
{
	switch (key)
	{
	case '8':
		if (vrtcldegree < 1)
			vrtcldegree += 0.1;
		break;
	case '2':
		if (vrtcldegree > -1)
			vrtcldegree -= 0.1;
		break;
	case '4':
		hrzndegree += 0.1;

		break;
	case '6':
		hrzndegree -= 0.1;
		break;
;
	case 'b':
		bim = !bim;
		printf("%d",bim);
		break;
	}


	glutPostRedisplay();
}


int main(int argc, char** argv)
{
	obj1 = ReadObject("../resource/chr_knight.obj");
	int n = 0;
	
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DEPTH | GLUT_SINGLE);
	glutInitWindowSize(1000, 1000);
	glutInitWindowPosition(300, 200);
	glClearColor(0.0, 0.0, 0.0, 0.0);//rgba (a: 투명도)


	glutCreateWindow("Lighting");
	init_light();// window 생성후 빛 초기화

	glutDisplayFunc(Display);
	glutReshapeFunc(Reshape);
	glutKeyboardFunc(keyboard);
	glutMainLoop();

	return 0;

}