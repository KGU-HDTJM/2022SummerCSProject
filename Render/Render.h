#include "LinearAlgebra.h"
#include "HDTJMType.h"
#define TRIANGLES 3
#define QAUDS 4

int viewX = 0, viewY = 0;
void VertexFaceMapping(int* face, int facelen, Vector3f_t* vertex, Vector3f_t* vertexBuffer);

//Texture
GLubyte* LoadBmp(const char* imagePath, int* width, int* height);

void InitTexture(unsigned char* data, int width, int height);
void TextureMapping(Vector2f_t* textureCoord, Vector2f_t* textureBuffer, int vertexCount, int textureCount);

void Reshape(float width, float height, float winSizeW, float winSizeH);

void RenderBuffer();
