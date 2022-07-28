#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define GET_VERTEX_LENGTH_BUFFER_SIZE 0x80

int main(void)
{
	FILE* fp = fopen("C:/Users/dahyu/source/repos/HDJM/resource/testio.obj", "r");//파일 오픈
	char buf[GET_VERTEX_LENGTH_BUFFER_SIZE] = { 0, };
	int element_vertex = 0; //반복할 횟수 저장할 곳 == aaa파일에서 72
	char element[9];
	char vertex[7];

	float xyz[18][4][3] = { 0 };

	int i = 0;

	float x = 0, y = 0, z = 0; //파일 불렀을 때 x,y,z값
	int red = 0, green = 0, blue = 0; //파일 불렀을 때 rgb값

	int element_vertex_count = 0; //4째줄 찾기 위한 거

	if (!fp) //파일없으면 에러처리
	{
		printf("파일 없다.\n");
		return -1;
	}


	while (fgets(buf, GET_VERTEX_LENGTH_BUFFER_SIZE, fp) != NULL)
	{
		element_vertex_count++;
		if (element_vertex_count == 3)
		{
			fscanf(fp, "%s %s %d\n", element, vertex, &element_vertex);
			printf("%s", buf); //불러온거 확인용
			printf("%d", element_vertex);
		}

		if (element_vertex_count == 12) //end_header다음줄 가져오기 위해서
		{
			printf("%s", buf);
			fgets(buf, GET_VERTEX_LENGTH_BUFFER_SIZE, fp);
			for (i = 0; i < element_vertex; i++)
			{
				fscanf(fp, "%f %f %f %d %d %d\n", &x, &y, &z, &red, &green, &blue);
				for (int j = 0; j < 4; j++)
				{
					xyz[i][j][0] = x;
					xyz[i][j][1] = y;
					xyz[i][j][2] = z;
				}
			}
			for (i = 0; i < element_vertex; i++)
			{
				for (int j = 0; j < 4; j++)
				{
					printf("%f %f %f\n", xyz[i][j][0], xyz[i][j][1], xyz[i][j][2]);
				}
			}
		}

	}
	fclose(fp);//파일 닫기


	return 0;
}