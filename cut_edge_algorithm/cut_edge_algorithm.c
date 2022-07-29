﻿#define _CRT_SECURE_NO_WARNINGS

#include <GL/GLUT.h>
#include <stdio.h>
#include <math.h>

#include "HDTJMType.h"
#include "HDTJMDef.h"
#include "LinearAlgebra.h"

#define EPSILON_F 1.19e-07F

#define LEFT -1.0F
#define RIGHT 1.0F
#define TOP 1.0F
#define BOTTOM -1.0F

#define EqualFloat(a, b) (fabs((a) - (b)) < EPSILON_F)
#define Min(a, b) ((a) < (b) ? (a) : (b))
#define Max(a, b) ((a) < (b) ? (b) : (a))

typedef Vector2f_t Point;

typedef struct Line
{
	Point P1;
	Point P2;
}Line;

const Line SQUARE_RANGE[4] = {
	{ { LEFT,	TOP		},		{ RIGHT,	TOP		} },
	{ { RIGHT,	TOP		},		{ RIGHT,	BOTTOM	} },
	{ { RIGHT,	BOTTOM	},		{ LEFT,		BOTTOM	} },
	{ { LEFT,	BOTTOM	},		{ LEFT,		TOP		} }
};

#define W_H 500
#define W_W 500


// 김민재 코딩표준좀 지켜 나중에는 없고 이  코드 읽게되는 시간이후로 당장
// 코드 고치면 태윤이하고 같이 읽어보라고 시킬껀데 못봤다고 하면 200% 변명으로 받아들일꺼임

// float 상수 뒤에 F붙이는거 대소문자 신경써주고

int bWire = 1;

float TriPosX;
float TriPosY;
float Size = 0.5;
//Object obj1;

void DrawSq(void)
{
	Vector3f_t vertex[4] =
	{
		{ RIGHT,	TOP,	1.0F},
		{ RIGHT,	BOTTOM, 1.0F},
		{ LEFT,		BOTTOM, 1.0F},
		{ LEFT,		TOP,	1.0F}
	};

	GLint Face[4] = { 0,1,2,3 };

	glVertexPointer(3, GL_FLOAT, 0, vertex);
	glDrawElements(GL_QUADS, 4, GL_UNSIGNED_INT, Face);
}

// 함수이름 좀더 명시적으로, 수식에서 가져온거라면 주석으로 수식의 출처와 내용을 간략하게 적어주면 좋을꺼 같음
int Direction(const Point* a, const Point* b, const Point* c)
{
	/* ---H가 추가-- */
	// 다른 방법으로 분기없이 계산 가능 할꺼 같지만 테이블 참조하는게 제일 코드가 간결해지는거 같음
	const static int RET_TABLE[2][2] = { { 0, 0 }, { 1, -1 } };
	/* ---H가 추가--- */
	Matrix2_t mat = GetMatrix2(a->X - c->X, a->Y - c->Y, b->X - c->X, b->Y - c->Y);
	float value = Det2(mat);
	int bNegNum = (*(int*)&value) >> 31;
	int temp = EqualFloat(value, 0.F);
	/* ---H가 추가--- */

	return RET_TABLE[temp][bNegNum];
	/* ---H가 추가--- */
	// 분기가 너무 많음 (최적화 힘들어져)
	// truth table
	// row: cross == 0, col: cross > 0
	//			true	false
	// true		0		1
	// false	0		-1
	// return crossProduct == 0 ? 0 : (crossProduct > 0 ? 1 : -1);
}


boolean_t OnSegment(Point* a, Point* b, Point* c)
{
	/*
	* 불필요한 분기 False = 0, True = 1이니까 바로 연산결과를 리턴하면됨
	if (c.x >= min(a.x, b.x) && c.x <= max(a.x, b.x) && c.y >= min(a.y, b.y) && c.y <= max(a.y, b.y))
		return True;
	return False;
	*/
	return c->X >= Min(a->X, b->X)
		&& c->X <= Max(a->X, b->X)
		&& c->Y >= Min(a->Y, b->Y)
		&& c->Y <= Max(a->Y, b->Y);
}

boolean_t IsIntersected(Line* l1, Line* l2)
{
	int d1, d2, d3, d4;

	d1 = Direction(&l2->P1, &l2->P2, &l1->P1);
	d2 = Direction(&l2->P1, &l2->P2, &l1->P2);

	d3 = Direction(&l1->P1, &l1->P2, &l2->P1);
	d4 = Direction(&l1->P1, &l1->P2, &l2->P2);
	if (d1 * d2 < 0 && d3 * d4 < 0) { return True; }

	if (d1 == 0 && OnSegment(&l2->P1, &l2->P2, &l1->P1)) { return True; }
	if (d2 == 0 && OnSegment(&l2->P1, &l2->P2, &l1->P2)) { return True; }
	if (d3 == 0 && OnSegment(&l1->P1, &l1->P2, &l2->P1)) { return True; }
	if (d4 == 0 && OnSegment(&l1->P1, &l1->P2, &l2->P2)) { return True; }
	return False;

}

boolean_t Comparator(Point* left, Point* right)
{
	// 코드 한줄을 적더라도 중괄호 열기
	// floating number의 비교는 == 쓰면 상수값끼리의 비교가 아닌이상 정확하지 않음 오차를 생각하여\
	// 대소관계를 사용하거나 epsilon delta 비교를 해야함
	/*
	if (left->X == right->X) { return left->Y <= right->Y; }
	return left->X <= right->X;
	*/
	if (EqualFloat(left->X, right->X)) { return left->Y <= right->Y; }
	return left->X <= right->X;
}

// 이런건 매크로 함수로 하는게 좋을꺼 같음 함수호출 비용이 Swap 연산보다 클 가능성이 있음
void Swap(Point* p1, Point* p2)
{
	Point temp;
	temp = *p1;
	*p1 = *p2;
	*p2 = temp;
}

boolean_t LineIntersection(Line* l1, Line* l2) {
	boolean_t bIsIntersected;
	int l1_l2 = OnSegment(&l1->P1, &l1->P2, &l2->P1) * OnSegment(&l1->P1, &l1->P2, &l2->P2);
	int l2_l1 = OnSegment(&l2->P1, &l2->P2, &l1->P1) * OnSegment(&l2->P1, &l2->P2, &l1->P2);

	if (l1_l2 == 0 && l2_l1 == 0)
	{
		if (Comparator(&l1->P2, &l1->P1)) { Swap(&l1->P1, &l1->P2); }
		if (Comparator(&l2->P2, &l2->P1)) { Swap(&l2->P1, &l2->P2); }

		bIsIntersected = Comparator(&l2->P1, &l1->P2) && Comparator(&l1->P1, &l2->P2);
	}
	else { bIsIntersected = (l1_l2 <= 0) && (l2_l1 <= 0); }
	return bIsIntersected;
}

void ReviseVertex(Vector3f_t* vertex)
{
	for (int i = 0; i < 3; i++)
	{
		Line triSide = 
		{
			{ vertex[i].V[0],				vertex[i].V[1]			 },
			{ vertex[(i + 1) % 3].V[0],		vertex[(i + 1) % 3].V[1] }
		};
		//printf("%.1f %.1f\n", triSide.P1.X, triSide.P1.Y);
		for (int j = 0; j < 4; j++)
		{
			Line sqrSide = SQUARE_RANGE[j];
			float d = (triSide.P1.X - triSide.P2.X) * (sqrSide.P1.Y - sqrSide.P2.Y)
				- (triSide.P1.Y - triSide.P2.Y) * (sqrSide.P1.X - sqrSide.P2.X);
			if (IsIntersected(&triSide, &sqrSide) && !EqualFloat(d, 0.F))
			{
				Point point = {
					((triSide.P1.X * triSide.P2.Y - triSide.P1.Y * triSide.P2.X) * (sqrSide.P1.X - sqrSide.P2.X)
					- (triSide.P1.X - triSide.P2.X) * (sqrSide.P1.X * sqrSide.P2.Y - sqrSide.P1.Y * sqrSide.P2.X)) / d,
					((triSide.P1.X * triSide.P2.Y - triSide.P1.Y * triSide.P2.X) * (sqrSide.P1.Y - sqrSide.P2.Y)
					- (triSide.P1.Y - triSide.P2.Y) * (sqrSide.P1.X * sqrSide.P2.Y - sqrSide.P1.Y * sqrSide.P2.X)) / d
				};
				printf("%.1f %.1f\n", point.X, point.Y);
			}
		}
	}
}

void DrawTri(float x, float y, float Size, float angle)
{
	// 기본적으로 컴마(,)로 구분되는 값은 띄어쓰기로 띄어줘
	float vertex[3][3] = {
		{ 0.0F * Size + x,	1.0F * Size + y,	1.0F },
		{ 0.0F * Size + x,	-1.0F * Size + y,	1.0F },
		{ 1.0F * Size + x,	0.0F * Size + y,	1.0F },
	};

	GLint Face[3] = { 0,1,2 };
	glVertexPointer(3, GL_FLOAT, 0, vertex);
	ReviseVertex(vertex);
	glRotatef(angle, 0.0, 0.0, 1.0);


	glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_INT, Face);
}

void Display(void)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	gluLookAt(0, 0, 1, 0.0, 0.0, 0.0, 0, 1, 0);
	if (bWire)glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glPushMatrix();

	DrawSq();
	glPopMatrix();
	glPushMatrix();
	DrawTri(TriPosX, TriPosY, Size, 0);
	glPopMatrix();
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

void keyboard(unsigned char key, int x, int y)
{
	// 이렇게 하면 대소문자 둘다 커버 가능
	switch (key | 0x20)
	{
	case 'w':
		TriPosY += 0.1;
		break;
	case 'a':
		TriPosX -= 0.1;
		break;
	case 's':
		TriPosY -= 0.1;
		break;
	case 'd':
		TriPosX += 0.1;
		break;
	case 'e':
		bWire = !bWire;
	default:
		break;
	}


	glutPostRedisplay();
}

int main(int argc, char** argv)
{

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DEPTH | GLUT_SINGLE);
	glutInitWindowSize(1000, 1000);
	glutInitWindowPosition(300, 200);
	glClearColor(0.0, 0.0, 0.0, 0.0);//rgba (a: 투명도)

	glutCreateWindow("Lighting");
	glEnableClientState(GL_VERTEX_ARRAY);
	glutDisplayFunc(Display);
	glutReshapeFunc(Reshape);
	glutKeyboardFunc(keyboard);
	glutMainLoop();

	return 0;

}
/*
void ReviseVertex(float vertex[3][3])
{
	for (int i = 0; i < 3; i++)
	{
		Line triSide = {
			{vertex[i][0],vertex[i][1]},
			{vertex[(i + 1) % 3][0],vertex[(i + 1) % 3][1]}
		};
		//printf("%.1f %.1f\n", triSide.p1.x, triSide.p1.y);
		for (int j = 0; j < 4; j++)
		{
			Line sqrSide = square_s[j];
			float d = (triSide.p1.x - triSide.p2.x) * (sqrSide.p1.y - sqrSide.p2.y)
				- (triSide.p1.y - triSide.p2.y) * (sqrSide.p1.x - sqrSide.p2.x);
			if (IsIntersected(triSide, sqrSide) && d != 0.0f)
			{
				Point point = {
					((triSide.p1.x * triSide.p2.y - triSide.p1.y * triSide.p2.x) * (sqrSide.p1.x - sqrSide.p2.x)
					- (triSide.p1.x - triSide.p2.x) * (sqrSide.p1.x * sqrSide.p2.y - sqrSide.p1.y * sqrSide.p2.x)) / d,
					((triSide.p1.x * triSide.p2.y - triSide.p1.y * triSide.p2.x) * (sqrSide.p1.y - sqrSide.p2.y)
					- (triSide.p1.y - triSide.p2.y) * (sqrSide.p1.x * sqrSide.p2.y - sqrSide.p1.y * sqrSide.p2.x)) / d
				};

				printf("%.1f %.1f\n", point.x, point.y);
			}
		}
	}
}
void DrawTri(float x, float y, float size, float angle)
{
	float vertex[3][3] = {
		{0.0f * size + x,1.0f * size + y,1.0f},
		{0.0f * size + x,-1.0f * size + y,1.0f},
		{1.0f * size + x,0.0f * size + y,1.0f},
	};

	GLint Face[3] = { 0,1,2 };
	glVertexPointer(3, GL_FLOAT, 0, vertex);
	ReviseVertex(vertex);
	glRotatef(angle, 0.0, 0.0, 1.0);


	glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_INT, Face);
}

void Display(void)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	gluLookAt(0, 0, 1, 0.0, 0.0, 0.0, 0, 1, 0);
	if (wire)glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glPushMatrix();

	DrawSq();
	glPopMatrix();
	glPushMatrix();
	DrawTri(tirPosx, tirPosy, size, 0);
	glPopMatrix();
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

void keyboard(unsigned char key, int x, int y)
{
	switch (key)
	{
	case 'w':
		tirPosy += 0.1;
		break;
	case 'a':
		tirPosx -= 0.1;
		break;
	case 's':
		tirPosy -= 0.1;
		break;
	case 'd':
		tirPosx += 0.1;
		break;

	default:
		break;
	}


	glutPostRedisplay();
}




int main(int argc, char** argv)
{

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DEPTH | GLUT_SINGLE);
	glutInitWindowSize(1000, 1000);
	glutInitWindowPosition(300, 200);
	glClearColor(0.0, 0.0, 0.0, 0.0);//rgba (a: 투명도)

	glutCreateWindow("Lighting");
	glEnableClientState(GL_VERTEX_ARRAY);
	glutDisplayFunc(Display);
	glutReshapeFunc(Reshape);
	glutKeyboardFunc(keyboard);
	glutMainLoop();

	return 0;

}
*/