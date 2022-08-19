#define _CRT_SECURE_NO_WARNINGS
#include <GL/freeglut.h>
#include <stdio.h>
#include "Render.h"
#include "HDTJMDef.h"
#include "HDTJMType.h"
#include "LinearAlgebra.h"
#include "StackAlloc.h"


void VertTrans(Vector3f_t* vertex, size_t vertexSize,float posx,float posy,float posz)
{
	for (int i = 0 ; i < vertexSize ; i++)
	{
		vertex[i].X += posx;
		vertex[i].X += posy;
		vertex[i].X += posz;
	}
}
void VertRotafe(Vector3f_t* vertex, size_t vertexSize, float Angle, boolean_t axisX, boolean_t axisY, boolean_t axisZ)
{
	Matrix3_t mX, mY, mZ; // mX X축 기준 회전
	double rad;
	float sine, cosine;
	float posGx = 0, posGy = 0, posGz = 0;
	float vx, vy;
	/*posGx = (vertex[0].X + vertex[1].X + vertex[2].X) / 3;
	posGy = (vertex[0].Y + vertex[1].Y + vertex[2].Y) / 3;*/

	for (int i = 0; i < vertexSize; i++)
	{
		posGx += vertex[i].X;
		posGy += vertex[i].Y;
		posGz += vertex[i].Z;
	}
	posGx /= 3; posGy /= 3; posGy /= 3;

	vertex[0].X -= posGx; vertex[1].X -= posGx; vertex[2].X -= posGx;
	vertex[0].Y -= posGy; vertex[1].Y -= posGy; vertex[2].Y -= posGy;

	rad = TO_RAD(Angle);
	sine = (float)sin(rad); cosine = (float)cos(rad);

	mZ._11 = cosine;	mZ._12 = -sine;		mZ._13 = posGx;
	mZ._21 = sine;		mZ._22 = cosine;	mZ._23 = posGy;
	mZ._31 = 0;			mZ._32 = 0;			mZ._33 = 1.F;

	mY._11 = cosine;	mY._12 = posGx;			mY._13 = sine;
	mY._21 = 0;			mY._22 = 1.F;		mY._23 = 0;
	mY._31 = -sine;		mY._32 = posGz;			mY._33 = cosine;

	mX._11 = 1.F;		mX._12 = 0;			mX._13 = 0;
	mX._21 = posGy;			mX._22 = cosine;	mX._23 = -sine;
	mX._31 = posGz;			mX._32 = sine;		mX._33 = cosine;

	if (axisX)
	{
		for (int i = 0; i < vertexSize; i++)
		{
			vx = vertex[i].X;
			vy = vertex[i].Y;
			vertex[i].X = mZ._11 * vx + mZ._12 * vy + mZ._13;
			vertex[i].Y = mZ._21 * vx + mZ._22 * vy + mZ._23;
		}
	}
	
	if (axisX)
	{
		for (int i = 0; i < vertexSize; i++)
		{
			vx = vertex[i].X;
			vy = vertex[i].Y;
			vertex[i].X = mZ._21 * vy + mZ._22 * vy + mZ._23;
			vertex[i].Y = mZ._31 * vy + mZ._32 * vy + mZ._33;
		}
	}

	if (axisX)
	{
		for (int i = 0; i < vertexSize; i++)
		{
			vx = vertex[i].X;
			vy = vertex[i].Y;
			vertex[i].X = mZ._11 * vx + mZ._12 * vy + mZ._13;
			vertex[i].Y = mZ._21 * vx + mZ._22 * vy + mZ._23;
		}
	}

}
void VertScale(Vector3f_t* vertex, size_t vertexSize, float scale )
{
	for (int i = 0; i < vertexSize; i++)
	{
		vertex[i].X *= scale;
		vertex[i].Y *= scale;
		vertex[i].Z *= scale;
	}
}

//Vertex
void VertexFaceMapping(int* face, size_t facelen, Vector3f_t* vertex, Vector3f_t* vertexBuffer)
{
	for (int i = 0; i < facelen; i++)
	{
		vertexBuffer[i].X = vertex[face[i]].X;
		vertexBuffer[i].Y = vertex[face[i]].Y;
		vertexBuffer[i].Z = vertex[face[i]].Z;
	}
}
//Texture
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


//View
void ViewCam(float vrtciDegree,float hrznDegree,float R)
{
	const float camy = (GLfloat)R * (GLfloat)sin(vrtciDegree);
	const float camx = (GLfloat)pow(R * R + camy * camy, (GLfloat)0.5) * (GLfloat)cos(hrznDegree);
	const float camz = (GLfloat)pow(R * R + camy * camy, (GLfloat)0.5) * (GLfloat)sin(hrznDegree);
	gluLookAt(camx, camy, camz, 0, 0, 0, 0, 1, 0);

}

void Reshape(float width, float height, float winSizeW, float winSizeH)
{
	glViewport(0, 0, width, height);
	GLfloat factor_w = (GLfloat)width / (GLfloat)winSizeW;
	GLfloat factor_h = (GLfloat)height / (GLfloat)winSizeH;

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(-1 * factor_w, 1 * factor_w, -1 * factor_h, 1 * factor_h, -10, 10);

}

//Render 
void WireFrameMode()
{glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);}
void SoildFrameMode()
{glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);}
void Render(Vector3f_t* vertexBuffer, Vector2f_t* textureBuffer ,size_t faceAngleCount, size_t faceSize)
{
	float** renderBuffer;
	renderBuffer = (float**)malloc(sizeof(float*) * 5);
	renderBuffer[0] = (float*)malloc(sizeof(float) * faceSize * 5);
	for (int i = 1; i < 5; i++) {
		renderBuffer[i] = renderBuffer[i - 1] + faceSize;
	}


	for (int i = 0; i < faceSize; i++)
	{
		renderBuffer[i][0] = textureBuffer[i].X;
		renderBuffer[i][1] = textureBuffer[i].Y;
		renderBuffer[i][2] = vertexBuffer[i].X;
		renderBuffer[i][3] = vertexBuffer[i].Y;
		renderBuffer[i][4] = vertexBuffer[i].Z;

	}
	glInterleavedArrays(GL_T2F_V3F,0, renderBuffer);
	

	if (faceAngleCount == TRIANGLES)
	{
		glDrawArrays(GL_TRIANGLES,0, faceSize);
	}
	else if (faceAngleCount == QAUDS)
	{
		glDrawArrays(GL_QUADS,0,faceSize);
	}

	free(renderBuffer[0]);
	free(renderBuffer);
}