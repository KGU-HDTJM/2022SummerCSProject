#define _CRT_SECURE_NO_WARNINGS 
#include <stdio.h>
#include <GL/freeglut.h>
#include "Object.h"


Object ReadObject(char* stream)
{
	Object Model;
	Model.Size = 0;
	Model.Worldx = 0;
	Model.Worldy = 0;
	Model.FaceNum = Model.VertexNum = Model.NomalNum  = Model.TexNum =  0;
	
	FILE* fp;


	char LineReader[100];

	fp = fopen(stream, "r");

	while (!feof(fp))
	{
		fscanf(fp, "%s", LineReader);

		if (LineReader[0] == 'v' && LineReader[1] == 'n' && LineReader[2] == '\0')
			Model.NomalNum++;
		else if (LineReader[0] == 'v' && LineReader[1] == 't' && LineReader[2] == '\0')
			Model.TexNum++;
		else if (LineReader[0] == 'v' && LineReader[1] == '\0')
			Model.VertexNum++;
		else if (LineReader[0] == 'f' && LineReader[1] == '\0')
			Model.FaceNum++;
		

		memset(LineReader, '\0', sizeof(LineReader));
	}

	Model.VertexArr = (GLfloat*)malloc(sizeof(GLfloat) * (Model.VertexNum * 3));
	Model.NomalArr = (Nomal*)malloc(sizeof(Nomal) * (Model.NomalNum * 3));

	

	Model.Texcoords = (GLfloat*)malloc(sizeof(GLfloat) * (Model.TexNum * 2));
	Model.FaceArr.v = (GLint*)malloc(sizeof(GLint) * (Model.FaceNum * 3));
	Model.FaceArr.vn = (GLint*)malloc(sizeof(GLint) * (Model.FaceNum * 3));
	Model.FaceArr.vt = (GLint*)malloc(sizeof(GLint) * (Model.FaceNum * 3));

	int Vidx = 0;
	int Fidx = 0;
	int Nidx = 0;
	int Tidx = 0;
	int vt = 0;
	int vn = 0;
	
	rewind(fp);
	while (!feof(fp))
	{
		fscanf(fp, "%s", LineReader);
		if (LineReader[0] == 'v' && LineReader[1] == 'n' && LineReader[2] == '\0')
		{
			fscanf(fp, "%f %f %f", &(Model.NomalArr[Nidx].nx), &(Model.NomalArr[Nidx].ny), &(Model.NomalArr[Nidx].nz));
			Nidx++;
		}
		else if (LineReader[0] == 'v' && LineReader[1] == 't' && LineReader[2] == '\0')
		{
			fscanf(fp, "%f %f", &(Model.Texcoords[Tidx]), &(Model.Texcoords[Tidx + 1]));
			Tidx += 2;
		}
		else if (LineReader[0] == 'v' && LineReader[1] == '\0')
		{
			fscanf(fp, "%f %f %f", &(Model.VertexArr[Vidx]), &(Model.VertexArr[Vidx + 1]), &(Model.VertexArr[Vidx + 2]));

			Vidx += 3;
		}
		else if (LineReader[0] == 'f' && LineReader[1] == '\0')
		{
			fscanf(fp, "%i/%i/%d %i/%i/%d %i/%i/%d",
				&(Model.FaceArr.v[Fidx]), &(Model.FaceArr.vt[Fidx]), &(Model.FaceArr.vn[Fidx]),
				&(Model.FaceArr.v[Fidx + 1]), &(Model.FaceArr.vt[Fidx + 1]), &(Model.FaceArr.vn[Fidx + 1]),
				&(Model.FaceArr.v[Fidx + 2]), &(Model.FaceArr.vt[Fidx + 2]), &(Model.FaceArr.vn[Fidx + 2])
			);
			Model.FaceArr.v[Fidx] -= 1;
			Model.FaceArr.v[Fidx + 1] -= 1;
			Model.FaceArr.v[Fidx + 2] -= 1;

		
			Model.FaceArr.vt[Fidx] -= 1;
			Model.FaceArr.vt[Fidx + 1] -= 1;
			Model.FaceArr.vt[Fidx + 2] -= 1;

			Model.FaceArr.vn[Fidx] -= 1;
			Model.FaceArr.vn[Fidx + 1] -= 1;
			Model.FaceArr.vn[Fidx + 2] -= 1;
			Fidx += 3;
		}

		memset(LineReader, '\0', sizeof(LineReader));

	}

	Model.Nomalbuffer = (GLfloat*)malloc(sizeof(GLfloat) * (Model.FaceNum * 9));
	
	Nidx = 0;
	for (int i = 0; i < Model.FaceNum*3; i++) 
	{
		Model.Nomalbuffer[Nidx] = Model.NomalArr[Model.FaceArr.vn[i]].nx;
		Model.Nomalbuffer[Nidx + 1] = Model.NomalArr[Model.FaceArr.vn[i]].ny;
		Model.Nomalbuffer[Nidx + 2] = Model.NomalArr[Model.FaceArr.vn[i]].nz;
		//printf("%f %f %f \n", Model.Nomalbuffer[Nidx], Model.Nomalbuffer[Nidx + 1], Model.Nomalbuffer[Nidx + 2]);
		Nidx += 3;
	}

	

	fclose(fp);

	return Model;
}

void DrawObject(Object obj)
{
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_NORMAL_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);

	glVertexPointer(3, GL_FLOAT, 0, obj.VertexArr);
	glNormalPointer(GL_INT,0,obj.Nomalbuffer);
	glTexCoordPointer(2,GL_FLOAT,0 ,obj.Texcoords);

	glDrawElements(GL_TRIANGLES, obj.FaceNum * 3, GL_UNSIGNED_INT, obj.FaceArr.v);
}
