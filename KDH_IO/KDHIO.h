#pragma once

typedef union
{
	struct
	{
		byte_t Red;
		byte_t Green;
		byte_t Blue;
		byte_t Alpha;
	};
	int rgba;
} RGBA_t;



void StoreVertex(
	Vector3f_t* outVertex, FILE* fp, int* colorIndex,
	RGBA_t* colorArr, int* vertexIndexArr, int elementVertex
);