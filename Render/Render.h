#include "LinearAlgebra.h"
#include "HDTJMType.h"
#define TRIANGLES 3
#define QAUDS 4
#define PI 3.141592F
#define WINDOW_W 1000
#define WINDOW_H 1000

GLint curX = 0, curY = 0;
GLfloat R = 0.7;
GLfloat camx = 0, camy = 0, camz = 1;
GLfloat hrzndegree = 0, vrtcldegree = 0;


void VertTrans(Vector3f_t* vertex, size_t vertexSize, float posx, float posy, float posz);
void VertRotafe(Vector3f_t* vertex, size_t vertexSize, float Angle, boolean_t axisX, boolean_t axisY, boolean_t axisZ);
void VertScale(Vector3f_t* vertex, size_t vertexSize, float scale);
void VertexFaceMapping(int* face, size_t facelen, Vector3f_t* vertex, Vector3f_t* vertexBuffer);

GLubyte* LoadBmp(const char* imagePath, int* width, int* height);
void InitTexture(unsigned char* data, int width, int height);

void TextureMapping(Vector2f_t* textureCoord, Vector2f_t* textureBuffer, size_t vertexCount, size_t textureCount);
void ViewCam(float vrtciDegree, float hrznDegree, float R);
void Reshape(float width, float height, float winSizeW, float winSizeH);
void WireFrameMode();
void SoildFrameMode();
void Render(Vector3f_t* vertexBuffer, Vector2f_t* textureBuffer, size_t faceEdgeCount, size_t faceSize);
