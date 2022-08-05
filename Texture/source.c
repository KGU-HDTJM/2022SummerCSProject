#define _CRT_SECURE_NO_WARNINGS
#include <GL/freeglut.h>
#include <stdio.h>
#include <math.h>
#include "LinearAlgebra.h"


#define W_H 500
#define W_W 500
int bim = 0;
int View = 1;

GLfloat R = 0.7;
GLfloat camx = 0, camy = 0, camz = 1;
GLfloat hrzndegree = 0, vrtcldegree = 0;

//GLubyte* LoadBmp(const char* imagePath, unsigned int* imageWidth, int* imageHeight);

void Display(void);
void init_light(void);
void keyboard(unsigned char key, int x, int y);
void Display(void);
void Reshape(int width, int height);
GLubyte* LoadBmp(const char* imagePath, int* width, int* height);
void InitTexture(unsigned char* data, int width, int height);
void CreateVoxel(GLfloat size, GLfloat posx, GLfloat posy, GLfloat posz);
void Modeling(void);


int main(int argc, char** argv)
{	



	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DEPTH | GLUT_SINGLE);
	glutInitWindowSize(1000, 1000);
	glutInitWindowPosition(300, 200);
	glClearColor(0.0, 0.0, 0.0, 0.0);//rgba (a: 투명도)

	glutCreateWindow("Lighting");
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	init_light();// window 생성후 빛 초기화


	// 은면제거  
	//glEnable(GL_CULL_FACE);
	

	int imageWidht, imageHeight;
	glEnable(GL_TEXTURE_2D);
	GLubyte* Texture = LoadBmp("test-pattern.bmp", &imageWidht, &imageHeight);
	InitTexture(Texture, imageWidht, imageHeight);

	glutDisplayFunc(Display);
	glutReshapeFunc(Reshape);
	glutKeyboardFunc(keyboard);
	glutMainLoop();

	return 0;

}


GLubyte* LoadBmp(const char* imagePath, int* width, int* height)
{
	// Data read from the header of the BMP file
	unsigned char header[54]; // Each BMP file begins by a 54-bytes header
	unsigned int dataPos;     // Position in the file where the actual data begins
	unsigned int imageSize;   // = width*height*3
	// Actual RGB data
	unsigned char* data;

	FILE* file = fopen(imagePath, "rb");
	if (!file) { printf("Image could not be opened\n"); return 0; }


	if (fread(header, 1, 54, file) != 54) { // If not 54 bytes read : problem
		printf("Not a correct BMP file\n");
		return 0;
	}

	if (header[0] != 'B' || header[1] != 'M') {
		printf("Not a correct BMP file\n");
		return 0;
	}

	// 바이트 배열에서 int 변수를 읽습니다. 
	dataPos = *(int*)&(header[0x0A]);
	imageSize = *(int*)&(header[0x22]);
	*width = *(int*)&(header[0x12]);
	*height = *(int*)&(header[0x16]);


	if (imageSize == 0)    imageSize = *width * *height * 3; // 3 : one byte for each Red, Green and Blue component
	if (dataPos == 0)      dataPos = 54; // The BMP header is done that way

	data = (GLubyte*)malloc(sizeof(GLubyte) * imageSize);

	// 파일에서 버퍼로 실제 데이터 넣기. 
	fread(data, 1, imageSize, file);

	//이제 모두 메모리 안에 있으니까, 파일을 닫습니다. 
	//Everything is in memory now, the file can be closed
	fclose(file);
	return data;
}

void InitTexture(unsigned char* data, int width, int height)
{
	GLuint tex_id = 1;
	glGenTextures(1, &tex_id);
	glBindTexture(GL_TEXTURE_2D, tex_id);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_BGRA_EXT, GL_UNSIGNED_BYTE, data);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	glEnable(GL_TEXTURE_2D);

}

void Modeling(void)
{
	CreateVoxel(0.2F, 0.6F, 0.0F, 0.0F);
	glDrawArrays(GL_QUADS, 0, 24);
	CreateVoxel(0.2F, 0.2F, 0.0F, 0.0F);
	glDrawArrays(GL_QUADS, 0, 24);
	CreateVoxel(0.2F,0.2F, 0.4F, 0.0F);
	glDrawArrays(GL_QUADS, 0, 24);
	CreateVoxel(0.2F,-0.2F, 0.4F, 0.0F);
	glDrawArrays(GL_QUADS, 0, 24);
}

void VoxelVertex(Vector3f_t* verts, Vector3f_t* vertexBuffer,int* Face,int faceLen)
{
	for (int i = 0; i < faceLen; i++)
	{
		vertexBuffer[i] = verts[Face[i]];
	}

}

void CreateVoxel(GLfloat size, GLfloat posx, GLfloat posy, GLfloat posz)
{
	

	GLfloat modelBuf[24][5] = {
		//앞면
		//Tw	Th		Vx		Vy		Vz
		{0.0F , 0.0F , -1.0F* size + posx, -1.0F* size + posy,  1.0F* size + posz},
		{1.0F , 0.0F ,  1.0F* size + posx, -1.0F* size + posy,  1.0F* size + posz},
		{1.0F , 1.0F ,  1.0F* size + posx,  1.0F* size + posy,  1.0F* size + posz},
		{0.0F , 1.0F , -1.0F* size + posx,  1.0F* size + posy,  1.0F* size + posz},

		//뒷면
		{1.0F , 0.0F , -1.0F* size + posx, -1.0F* size + posy, -1.0F* size + posz},
		{1.0F , 1.0F , -1.0F* size + posx,  1.0F* size + posy, -1.0F* size + posz},
		{0.0F , 1.0F ,  1.0F* size + posx,  1.0F* size + posy, -1.0F* size + posz},
		{0.0F , 0.0F ,  1.0F* size + posx, -1.0F* size + posy, -1.0F* size + posz},
		
		//윗면
		{0.0F , 1.0F , -1.0F* size + posx,  1.0F* size + posy, -1.0F* size + posz},
		{0.0F , 0.0F , -1.0F* size + posx,  1.0F* size + posy,  1.0F* size + posz},
		{1.0F , 0.0F ,  1.0F* size + posx,  1.0F* size + posy,  1.0F* size + posz},
		{1.0F , 1.0F ,  1.0F* size + posx,  1.0F* size + posy, -1.0F* size + posz},

		//바닥면
		{1.0F , 1.0F , -1.0F* size + posx, -1.0F* size + posy, -1.0F* size + posz},
		{0.0F , 1.0F ,  1.0F* size + posx, -1.0F* size + posy, -1.0F* size + posz},
		{0.0F , 0.0F ,  1.0F* size + posx, -1.0F* size + posy,  1.0F* size + posz},
		{1.0F , 0.0F , -1.0F* size + posx, -1.0F* size + posy,  1.0F* size + posz},

		//오른쪽 옆면
		{1.0F , 0.0F ,  1.0F* size + posx, -1.0F* size + posy, -1.0F* size + posz},
		{1.0F , 1.0F ,  1.0F* size + posx,  1.0F* size + posy, -1.0F* size + posz},
		{0.0F , 1.0F ,  1.0F* size + posx,  1.0F* size + posy,  1.0F* size + posz},
		{0.0F , 0.0F ,  1.0F* size + posx, -1.0F* size + posy,  1.0F* size + posz},

		//왼쪽 옆면
		{0.0F , 0.0F , -1.0F* size + posx, -1.0F* size + posy, -1.0F* size + posz},
		{1.0F , 0.0F , -1.0F* size + posx, -1.0F* size + posy,  1.0F* size + posz},
		{1.0F , 1.0F , -1.0F* size + posx,  1.0F* size + posy,  1.0F* size + posz},
		{0.0F , 1.0F , -1.0F* size + posx,  1.0F* size + posy, -1.0F* size + posz},

	};
	glInterleavedArrays(GL_T2F_V3F,0,modelBuf);

	
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

	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	
	
	Modeling();
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

	glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER, GL_TRUE);

	GLfloat light_global_ambient[] = { 0.3F,0.3F,0.3F,1.0F };
	GLfloat light_0_pos[] = { 2.0F , 2.0F , -2.0F , 1.0F };

	GLfloat light_0_ambient[] = { 1.0F , 1.0F , 1.0F , 1.0F };
	GLfloat light_0_diffuse[] = { 1.0F , 1.0F , 1.0F , 1.0F };
	GLfloat light_0_specular[] = { 1.0F ,1.0F , 1.0F, 1.0F };

	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, light_global_ambient);
	glEnable(GL_LIGHT0);


	glLightfv(GL_LIGHT0, GL_AMBIENT, light_0_ambient);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, light_0_diffuse);
	glLightfv(GL_LIGHT0, GL_SPECULAR, light_0_specular);
	glLightfv(GL_LIGHT0, GL_POSITION, light_0_pos);




	glDisable(GL_COLOR_MATERIAL);
	GLfloat matrial_0_ambient[] = { 0.3F,0.3F,0.3F,1.0F };
	GLfloat matrial_0_diffuse[] = { 1.0F,1.0F,1.0F,1.0F }; // 물체의 색상을 가장 직접적으로 결정
	GLfloat matrial_0_specular[] = { 1.0F , 1.0F ,1.0F ,1.0F };
	GLfloat matrial_0_shininess[] = { 25.0F };
	glMaterialfv(GL_FRONT, GL_AMBIENT, matrial_0_ambient);
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
	case 'b':
		bim = !bim;
		printf("%d", bim);
		break;

	}


	glutPostRedisplay();
}
