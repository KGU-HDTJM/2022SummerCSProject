#define _CRT_SECURE_NO_WARNINGS

#include <GL/glut.h>
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
#define TO_RAD(x) ((x) * 0.01744444)

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

float TriPosX = 0.F;
float TriPosY = 0.F;
float Size = 0.5F;
int Angle = 1;
Vector3f_t Vertex[3];
static Matrix3_t RotationMatrix;


Matrix3_t GetRotateMatrix(int zAngle, int yAngle, int xAngle)
{
	Matrix3_t m0, m1;
	double rad;
	float sine, cosine;

	rad = TO_RAD(zAngle);
	sine = (float)sin(rad); cosine = (float)cos(rad);
	m0._11 = cosine;	m0._12 = -sine;		m0._13 = 0;
	m0._21 = sine;		m0._22 = cosine;	m0._23 = 0;
	m0._31 = 0;			m0._32 = 0;			m0._33 = 1.F;

	rad = TO_RAD(yAngle);
	sine = (float)sin(rad); cosine = (float)cos(rad);
	m1._11 = cosine;	m1._12 = 0;			m1._13 = sine;
	m1._21 = 0;			m1._22 = 1.F;		m1._23 = 0;
	m1._31 = -sine;		m1._32 = 0;			m1._33 = cosine;

	m0 = MulMatrix3(&m0, &m1);

	rad = TO_RAD(xAngle);
	sine = (float)sin(rad); cosine = (float)cos(rad);
	m1._11 = 1.F;		m1._12 = 0;			m1._13 = 0;
	m1._21 = 0;			m1._22 = cosine;	m1._23 = -sine;
	m1._31 = 0;			m1._32 = sine;		m1._33 = cosine;

	m0 = MulMatrix3(&m0, &m1);

	return m0;
}


// 일단 평면으로  가정해서 만듬(,angle) 
void PivotRotate(int zAngle)
{
	Matrix3_t m0;
	double rad;
	float sine, cosine;
	float posGx, posGy;
	float vx, vy;
	posGx = (Vertex[0].X + Vertex[1].X + Vertex[2].X) / 3;
	posGy = (Vertex[0].Y + Vertex[1].Y + Vertex[2].Y) / 3;

	Vertex[0].X -= posGx; Vertex[1].X -= posGx; Vertex[2].X -= posGx;
	Vertex[0].Y -= posGy; Vertex[1].Y -= posGy; Vertex[2].Y -= posGy;

	rad = TO_RAD(zAngle);
	sine = (float)sin(rad); cosine = (float)cos(rad);
	m0._11 = cosine;	m0._12 = -sine;		m0._13 = posGx;
	m0._21 = sine;		m0._22 = cosine;	m0._23 = posGy;
	m0._31 = 0;			m0._32 = 0;			m0._33 = 1.F;

	for (int i = 0; i < 3; i++)
	{
		vx = Vertex[i].X;
		vy = Vertex[i].Y;
		Vertex[i].X = m0._11 * vx + m0._12 * vy + m0._13;
		Vertex[i].Y = m0._21 * vx + m0._22 * vy + m0._23;
	}


}
void DrawSq(void)
{
	static Vector3f_t vertex[4] =
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
	int bNegNum = value < 0;
	int temp = !EqualFloat(value, 0.F);
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

	return (d1 * d2 <= 0 && d3 * d4 <= 0)
		|| (d1 == 0 && OnSegment(&l2->P1, &l2->P2, &l1->P1))
		|| (d2 == 0 && OnSegment(&l2->P1, &l2->P2, &l1->P2))
		|| (d3 == 0 && OnSegment(&l1->P1, &l1->P2, &l2->P1))
		|| (d4 == 0 && OnSegment(&l1->P1, &l1->P2, &l2->P2));
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
				printf("%.3f %.3f\n", point.X, point.Y);
			}
		}
	}
	printf("\n");
}

// 함수의 기능을 좀더 작개 분리할 필요가있음, 차리리 3각형 vertex를 받아서 그걸로 3각형 그리는게 낫지 이런식으로 만들어서 그리는거 하지마

void DrawTri(float x, float y)
{
	// 기본적으로 컴마(,)로 구분되는 값은 띄어쓰기로 띄어줘
	for (size_t i = 0; i < 3; i++)
	{
		Vertex[i] = Sum3f(Vertex[i], GetVector3f(x, y, 0.F));
	}

	GLint Face[3] = { 0,1,2 };
	glVertexPointer(3, GL_FLOAT, 0, Vertex);
	ReviseVertex(Vertex);
	glRotatef(0, 0.0, 0.0, 1.0);


	// 채움 공간은 삼각형만 하면 되는거니까 PolygonMode설정은 삼각형에서만
	if (bWire) { glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); }
	else { glPolygonMode(GL_FRONT_AND_BACK, GL_FILL); }

	glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_INT, Face);
}

void DrawDot(Vector3f_t* vertex, float size) {
	float posGx = (vertex[0].X + vertex[1].X + vertex[2].X) / 3;
	float posGy = (vertex[0].Y + vertex[1].Y + vertex[2].Y) / 3;

	Vector3f_t dotvertex[4] =
	{
		{ posGx + 1.F * size,    posGy + 1.F * size,    1.0F},
		{ posGx + 1.F * size,    posGy - 1.F * size,    1.0F},
		{ posGx - 1.F * size,    posGy - 1.F * size,    1.0F},
		{ posGx - 1.F * size,    posGy + 1.F * size,    1.0F}
	};

	GLint Face[4] = { 0,1,2,3 };
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glVertexPointer(3, GL_FLOAT, 0, dotvertex);
	glDrawElements(GL_QUADS, 4, GL_UNSIGNED_INT, Face);
}

void Display(void)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	gluLookAt(0, 0, 1, 0.0, 0.0, 0.0, 0, 1, 0);
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glPushMatrix();

	DrawSq();
	glPopMatrix();
	glPushMatrix();

	DrawTri(TriPosX, TriPosY);
	glPopMatrix();
	glPushMatrix();

	DrawDot(Vertex, 0.01);
	glPopMatrix();
	glFlush();

	//for (size_t i = 0; i < 3; i++)
	//{
	//	printf("%llu: %f %f %f\n", i, Vertex[i].X, Vertex[i].Y, Vertex[i].Z);
	//}


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
	static Matrix3_t temp;
	// 이렇게 하면 대소문자 둘다 커버 가능
	switch (key | 0x20)
	{
	case 'w':
		TriPosX = 0.F;
		TriPosY = 0.1F;
		break;
	case 'a':
		TriPosX = -0.1F;
		TriPosY = 0.F;
		break;
	case 's':
		TriPosX = 0.F;
		TriPosY = -0.1F;
		break; 
	case 'd':
		TriPosX = 0.1F;
		TriPosY = 0.F;
		break;
	case 'r':
		bWire = !bWire;
		break;
	case 'e':
		TriPosX = TriPosY = 0.F;
		PivotRotate(-Angle, Vertex, 3);
		break;
	case 'q':
		TriPosX = TriPosY = 0.F;
		PivotRotate(Angle, Vertex, 3);
		break;
	default:
		break;
	}
	glutPostRedisplay();
}



int main(int argc, char** argv)
{
	Vertex[0] = GetVector3f(0.0F, 1.0F, 1.0F);
	Vertex[1] = GetVector3f(0.0F, -1.0F, 1.0F);
	Vertex[2] = GetVector3f(1.0F, 0.0F, 1.0F);
	for (size_t i = 0; i < 3; i++)
	{
		Vertex[i] = MulScalar3f(Size, Vertex[i]);
		for (size_t j = 0; j < 3; j++)
		{
			RotationMatrix.M[i][j] = (float)(i == j);
		}
	}

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DEPTH | GLUT_SINGLE);
	glutInitWindowSize(1000, 1000);
	glutInitWindowPosition(300, 200);
	glClearColor(0.0, 0.5, 0.0, 0.3);//rgba (a: 투명도)

	glutCreateWindow("Lighting");
	glEnableClientState(GL_VERTEX_ARRAY);
	glutDisplayFunc(Display);
	glutReshapeFunc(Reshape);
	glutKeyboardFunc(keyboard);
	glutMainLoop();

	return 0;

}