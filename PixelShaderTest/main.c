#pragma warning (disable: 4996)
#include <stdio.h>
#include <stdlib.h>

#include <Windows.h>
#include <GL/glew.h>
#include <GL/GL.h>
#include <AMD/GL/glext.h>

#include <GL/wglew.h>
#include <wingdi.h>

#include <CL/cl.h>
#include <CL/cl_gl.h>

#include <AMD/GL/glut.h>

#include <GL/GLU.h>


#include "HDTJMType.h"
#include "HDTJMDef.h"
//#include "HDTJMThread.h"
#include "HeapAlloc.h"
//#include "StackAlloc.h"
//#include "LinearAlgebra.h"

cl_platform_id	Platform;
cl_device_id	Device;
cl_context		Context;
cl_program		Program;
cl_command_queue CmdQueue;
cl_kernel		Kernel;
GLuint			VAO[3];
GLuint			VBO[6];
cl_mem			ClMemObjects[6];

char* ReadFileMalloc(FILE* fp, size_t* size)
{
	fseek(fp, 0, SEEK_END);
	*size = (size_t)ftell(fp);
	fseek(fp, 0, SEEK_SET);
	char* sourceBuf = HAlloc(*size + 1, False, NULL);
	sourceBuf[*size] = 0i8;
	fread(sourceBuf, sizeof(char), *size, fp);
	return sourceBuf;
}

void InitCL(void)
{
	char* programSource;
	char* programLog;
	size_t programSourceSize;
	size_t logSize;
	int errRet = clGetPlatformIDs(1, &Platform, NULL);
	if (errRet < 0)
	{
		printf("Couldn't identify a platform\n");
		exit(1);
	}
	errRet = clGetDeviceIDs(Platform, CL_DEVICE_TYPE_GPU, 1, &Device, NULL);
	if (errRet == CL_DEVICE_NOT_FOUND)
	{
		errRet = clGetDeviceIDs(Platform, CL_DEVICE_TYPE_CPU, 1, &Device, NULL);
	}
	if (errRet < 0)
	{
		printf("Couldn't access any devices\n");
		exit(1);
	}

	cl_context_properties clProps[7] =
	{
	  CL_GL_CONTEXT_KHR, (cl_context_properties)wglGetCurrentContext(),
	  CL_WGL_HDC_KHR, (cl_context_properties)wglGetCurrentDC(),
	  CL_CONTEXT_PLATFORM, (cl_context_properties)Platform, 0
	};

	Context = clCreateContext(clProps, 1, &Device, NULL, NULL, &errRet);
	if (errRet < 0)
	{
		printf("Couldn't create a context\n");
		exit(1);
	}
	FILE* clFile = fopen("red_shader.cl", "r");
	if (clFile == NULL)
	{
		printf("Couldn't founded kernel file\n");
		exit(1);
	}
	programSource = ReadFileMalloc(clFile, &programSourceSize);
	Program = clCreateProgramWithSource(Context, 1, &programSource, &programSourceSize, &errRet);
	if (errRet < 0)
	{
		printf("Couldn't create the program\n");
		exit(1);
	}
	HFree(programSource);
	fclose(clFile);
	errRet = clBuildProgram(Program, 0, NULL, NULL, NULL, NULL);
	if (errRet < 0)
	{
		clGetProgramBuildInfo(Program, Device, CL_PROGRAM_BUILD_LOG,
			0, NULL, &logSize);
		programLog = HAlloc(logSize + 1, False, NULL);
		programLog[logSize] = 0i8;
		clGetProgramBuildInfo(Program, Device, CL_PROGRAM_BUILD_LOG,
			logSize + 1, programLog, NULL);
		printf("%s\n", programLog);
		HFree(programLog);
		exit(1);
	}

	CmdQueue = clCreateCommandQueue(Context, Device, 0, &errRet);
	if (errRet < 0)
	{
		printf("Couldn't create a command queue\n");
		exit(1);
	}

	Kernel = clCreateKernel(Program, "RedShader", &errRet);
	if (errRet < 0)
	{
		printf("Couldn't create a kernel: %d\n", errRet);
		exit(1);
	}
}

void InitGL(int argc, char** argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
	glutInitWindowSize(300, 300);
	glutCreateWindow("CLGL Test");
	glClearColor(1.F, 1.F, 1.F, 1.F);

	GLenum errRet = glewInit();
	if (errRet != GLEW_OK)
	{
		printf("Couldn't initialize GLEW");
		exit(1);
	}
}

void ConfigureShaderData(void)
{
	int i, errRet;
	glGenVertexArrays(3, VAO);
	glBindVertexArray(VAO[0]);

	glGenBuffers(6, VBO);
	for (size_t i = 0; i < 6; i++)
	{
		glBindBuffer(GL_ARRAY_BUFFER, VBO[i]);
		glBufferData(GL_ARRAY_BUFFER, 12 * sizeof(GLfloat), NULL, GL_DYNAMIC_DRAW);
		glVertexAttribPointer(i % 2, 3, GL_FLOAT, GL_FALSE, 0, 0);
		glEnableVertexAttribArray(i % 2);

	}
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	
	/* Create memory objects from the VBOs */
	for (i = 0; i < 6; i++)
	{
		ClMemObjects[i] = clCreateFromGLBuffer(Context, CL_MEM_WRITE_ONLY, VBO[i], &errRet);
		if (errRet < 0)
		{
			perror("Couldn't create a buffer object from the VBO");
			exit(1);
		}
		errRet = clSetKernelArg(Kernel, i, sizeof(cl_mem), &ClMemObjects[i]);
		if (errRet < 0)
		{
			printf("Couldn't set a kernel argument");
			exit(1);
		}
	}
}

void ExcuteKernel(void)
{

	int errRet;
	cl_event kernelEvent;

	/* Complete OpenGL processing */
	glFinish();

	/* Execute the Kernel */
	errRet = clEnqueueAcquireGLObjects(CmdQueue, 6, ClMemObjects, 0, NULL, NULL);
	if (errRet < 0)
	{
		printf("Couldn't acquire the GL objects\n");
		exit(1);
	}

	errRet = clEnqueueTask(CmdQueue, Kernel, 0, NULL, &kernelEvent);
	if (errRet < 0)
	{
		printf("Couldn't enqueue the Kernel\n");
		exit(1);
	}

	errRet = clWaitForEvents(1, &kernelEvent);
	if (errRet < 0)
	{
		printf("Couldn't enqueue the Kernel\n");
		exit(1);
	}

	clEnqueueReleaseGLObjects(CmdQueue, 6, ClMemObjects, 0, NULL, NULL);
	clFinish(CmdQueue);
	clReleaseEvent(kernelEvent);
}

void Display(void)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	for (size_t i = 2; i >= 0; i--)
	{
		glBindVertexArray(VAO[i]);
		glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
	}
	glBindVertexArray(0);
	glutSwapBuffers();
}

void Reshape(int w, int h)
{
	glViewport(0, 0, (GLsizei)w, (GLsizei)h);
}

int main(int argc, char** argv)
{
	int i;

	InitGL(argc, argv);

	InitCL();

	ConfigureShaderData();

	ExcuteKernel();

	glutDisplayFunc(Display);
	glutReshapeFunc(Reshape);
	glutMainLoop();

	for (i = 0; i < 6; i++)
	{
		clReleaseMemObject(ClMemObjects[i]);
	}
	clReleaseKernel(Kernel);
	clReleaseCommandQueue(CmdQueue);
	clReleaseProgram(Program);
	clReleaseContext(Context);

	/* Deallocate OpenGL resources */
	glDeleteBuffers(6, VBO);
}
