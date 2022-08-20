#define _CRT_SECURE_NO_WARNINGS 

typedef struct {
	int* v;
	int* vn;
	int* vt;
}Face;
typedef struct {
	float nx;
	float ny;
	float nz;
}Nomal;
typedef struct {
	float Size;
	int VertexNum;
	int FaceNum;
	int NomalNum;
	int TexNum;

	float Worldx;
	float Worldy;

	Nomal* NomalArr;
	float* Nomalbuffer;
	float* Texcoords;
	float* VertexArr;
	Face FaceArr;

}Object;

Object ReadObject(char* stream);
void DrawObject(Object obj);
