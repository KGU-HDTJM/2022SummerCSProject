#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define GET_VERTEX_LENGTH_BUFFER_SIZE 0x80

int main(void)
{
	FILE* fp = fopen("C:/Users/dahyu/source/repos/HDJM/resource/testio.obj", "r");//���� ����
	char buf[GET_VERTEX_LENGTH_BUFFER_SIZE] = { 0, };
	int element_vertex = 0; //�ݺ��� Ƚ�� ������ �� == aaa���Ͽ��� 72
	char element[9];
	char vertex[7];

	float xyz[18][4][3] = { 0 };

	int i = 0;

	float x = 0, y = 0, z = 0; //���� �ҷ��� �� x,y,z��
	int red = 0, green = 0, blue = 0; //���� �ҷ��� �� rgb��

	int element_vertex_count = 0; //4°�� ã�� ���� ��

	if (!fp) //���Ͼ����� ����ó��
	{
		printf("���� ����.\n");
		return -1;
	}


	while (fgets(buf, GET_VERTEX_LENGTH_BUFFER_SIZE, fp) != NULL)
	{
		element_vertex_count++;
		if (element_vertex_count == 3)
		{
			fscanf(fp, "%s %s %d\n", element, vertex, &element_vertex);
			printf("%s", buf); //�ҷ��°� Ȯ�ο�
			printf("%d", element_vertex);
		}

		if (element_vertex_count == 12) //end_header������ �������� ���ؼ�
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
	fclose(fp);//���� �ݱ�


	return 0;
}