#define _CRT_SECURE_NO_WARNINGS
#include <AMD/GL/glut.h>
#include <GL/GL.h>

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "HDTJMDef.h"
#include "HDTJMType.h"
#include "LinearAlgebra.h"
#include "StackAlloc.h"
#include "Render.h"

#define HDTJM_TRIANGLES

// !
GLubyte* LoadBmp(const char* imagePath, int* width, int* height)
{
	// Data read from the header of the BMP file
	unsigned char header[54]; // Each BMP file begins by a 54-bytes header
	unsigned int dataPos;     // Position in the file where the actual data begins
	unsigned int imageSize;   // = width*height*3
	// Actual RGB data
	unsigned char* data;

	FILE* file = fopen(imagePath, "rb");
	if (!file) { printf("Image could not be opened\n"); return NULL; }


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
	fread(data, 1, imageSize, file); // bgr

	//이제 모두 메모리 안에 있으니까, 파일을 닫습니다. 
	//Everything is in memory now, the file can be closed
	fclose(file);
	return data;
}

// !
void InitTexture(byte_t* data, int width, int height)
{
	GLuint tex_id = 1;
	glGenTextures(1, &tex_id);
	glBindTexture(GL_TEXTURE_2D, tex_id);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_BGR_EXT, GL_UNSIGNED_BYTE, data);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glEnable(GL_TEXTURE_2D);
}

void TextureMapping(Vector2f_t* textureCoord, Vector2f_t* textureBuffer, size_t vertexCount, size_t textureCount)
{
	for (int i = 0; i < vertexCount; i++)
	{
		textureBuffer[i].X = textureCoord[i % textureCount].X;
		textureBuffer[i].Y = textureCoord[i % textureCount].Y;
	}
}

// ! ViewCam -> GetViewMatrix
void ViewCam(float vrtciDegree, float hrznDegree, float	r)
{
	GLfloat radhrz = (90 - hrzndegree / 180) * PI;
	GLfloat radvrt = ((90 - vrtcldegree) / 180) * PI;
	camx = (GLfloat)r * (GLfloat)cos(radvrt) * (GLfloat)cos(radhrz);
	camy = (GLfloat)r * (GLfloat)sin(radvrt);
	camz = (GLfloat)r * (GLfloat)cos(radvrt) * (GLfloat)sin(radhrz);
	gluLookAt(camx, camy, camz, 0, 0, 0, 0, 1, 0);
}

// !
void Reshape(int width, int height)
{
	glViewport(0, 0, width, height);
	GLfloat factor_w = (GLfloat)width / (GLfloat)sizeW;
	GLfloat factor_h = (GLfloat)height / (GLfloat)sizeH;

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(-1 * factor_w, 1 * factor_w, -1 * factor_h, 1 * factor_h, -10, 10);
}

//Render Mode
void WireFrameMode()
{
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
}

void SoildFrameMode()
{
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}

// PushRenderObject
void PushRenderObject(Vector3f_t* vertexBuffer, Vector2f_t* textureBufferOrNULL, Vector3f_t* normalBufferOrNULL, size_t faceSize)
{
	glVertexPointer(faceSize, GL_FLOAT, 0, vertexBuffer);
	if (normalBufferOrNULL != NULL) { glNormalPointer(GL_FLOAT, 0, normalBufferOrNULL); }
	if (textureBufferOrNULL != NULL) { glTexCoordPointer(faceSize, GL_FLOAT, 0, textureBufferOrNULL); }
	
#if defined(HDTJM_TRIANGLES)
	glDrawArrays(GL_TRIANGLES, 0, (GLsizei)faceSize);
#elif defined(HDTJM_QUADS)
	glDrawArrays(GL_QUADS, 0, (GLsizei)faceSize);
#else
	assert(False && "폴리곤의 단위 미지정");
#endif
}
