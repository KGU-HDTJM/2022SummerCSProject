#define STB_IMAGE_IMPLEMENTATION
#define _CRT_SECURE_NO_WARNINGS

#include <GL/freeglut.h>
#include <stdio.h>
#include <math.h>
#include <stb_image.h>
#include "../header/LinearAlgebra.h"

#define W_H 100
#define W_W 100
int bim = 1;

typedef struct
{
	Vector3f_t Vertex[8];
	int TextureId;
}Voxel_t;

//Object obj1;
GLfloat R = 0.7;
GLfloat camx = 0.5, camy = 0.5, camz = 0.5;
GLfloat hrzndegree = 0, vrtcldegree = 0;
void DrawVoxel(void)
{
	Vector3f_t vertex[8] = {
		{-1.0f,-1.0f,1.0f},
		{-1.0f,1.0f,1.0f},
		{1.0f,1.0f,1.0f},
		{1.0f,-1.0f,1.0f},
		{-1.0f,-1.0f,-1.0f},
		{-1.0f,1.0f,-1.0f},
		{1.0f,1.0f,-1.0f},
		{1.0f,-1.0f,-1.0f}
	};

	Vector2f_t texCoords[] = {
		0.0 , 1.0,
		0.0 , 0.0,
		1.0 , 0.0,
		1.0 , 1.0,
		
		0.0 , 0.0,
		1.0 , 0.0,
		1.0 , 1.0,
		
		0.0 , 1.0
	
		
		

	};

	GLint Face[24] = {
		0,3,2,1,
		3,7,6,2,
		0,4,7,3,
		1,2,6,5,
		4,5,6,7,
		0,1,5,4
	};
	
	glVertexPointer(3, GL_FLOAT, 0, vertex);
	glTexCoordPointer(2, GL_FLOAT, 0, texCoords);
	for (GLint i = 0; i < 6; i++) {
		
		glDrawElements(GL_POLYGON, 4, GL_UNSIGNED_INT, &Face[4 * i]);
	}


}
void init_texture(unsigned char * data,int width,int height) {
	GLuint tex_id = 1;
	glGenTextures(1,&tex_id);
	glBindTexture(GL_TEXTURE_2D,tex_id);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	
	glEnable(GL_TEXTURE_2D);

}
void Display(void)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	camy = R * sin(vrtcldegree);
	camx = pow(R * R + camy * camy, (double)0.5) * cos(hrzndegree);
	camz = pow(R * R + camy * camy, (double)0.5) * sin(hrzndegree);
	gluLookAt(camx, camy, camz, 0, 0, 0, 0, 1, 0);
	
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	DrawVoxel();
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
	GLfloat matrial_0_diffuse[] = {1,1,1,1.0}; // 물체의 색상을 가장 직접적으로 결정
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
	int imgWidth, imgHeight,imgCh;
	unsigned char* image = stbi_load("gray-rock.jpg",&imgWidth,&imgHeight,&imgCh,0);
	
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DEPTH | GLUT_SINGLE);
	glutInitWindowSize(1000, 1000);
	glutInitWindowPosition(300, 200);
	glClearColor(0.0, 0.0, 0.0, 0.0);//rgba (a: 투명도)

	glutCreateWindow("Lighting");
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	init_light();// window 생성후 빛 초기화
	init_texture(image, imgWidth, imgHeight);
	glutDisplayFunc(Display);
	glutReshapeFunc(Reshape);
	glutKeyboardFunc(keyboard);
	glutMainLoop();

	return 0;

}