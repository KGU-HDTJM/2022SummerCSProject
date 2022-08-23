#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <intrin.h>

#include <AMD/GL/glut.h>
#include <GL/GL.h>
#include <GL/GLU.h>

#include <CL/cl.h>
#include <CL/cl_gl.h>

#include "HDTJMType.h"
#include "HDTJMDef.h"
#include "HDTJMThread.h"
#include "HeapAlloc.h"
#include "StackAlloc.h"

#define HEAP_SIZE (size_t)(1 << 30)

int main(void)
{
	cl_platform_id platform = NULL;
	cl_device_id device_id = NULL;
	cl_context context = NULL;
	cl_command_queue commnadQueue = NULL;
	cl_program program = NULL;
	cl_kernel pixelShader = NULL;
	cl_mem textureSource = NULL;
	cl_mem result = NULL;
	cl_uint platformCount = 0;
	cl_uint deviceCount = 0;
	cl_int errorRet = 0;
	FILE* fp = NULL;

	if (InitMem(HEAP_SIZE) != CREATE_HEAP_SUCCESS) { return -1; }

	char* kernelSource = HAlloc(1 << 16, False, &kernelSource);



	ReleaseMainMem();
	return 0;
}
