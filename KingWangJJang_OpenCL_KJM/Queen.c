#define _CRT_SECURE_NO_WORNINGS
#include <stdlib.h>
#include <CL/cl.h>
#include <CL/Utils/Error.h>
#include <glut.h>
#include <GL/GL.h>
#include <stdio.h>

#include "HDTJMType.h"
#include "HDTJMDef.h"

#define MAX_SOURCE_SIZE (0x100000)
#define MAX_CL_BUFFER_SIZE 1 << 16
#define MAX_CL_SUB_BUFFER 4
#define SUB_UNIT 16384
#define CLGetAlignedSize(size) ((size + 15) & ~15)


typedef enum
{
	CLReadBuf, CLWriteBuf
} eCLBufferType_t;

static cl_platform_id* PlatformID = NULL;
static cl_uint NumPlatform;
static cl_device_id* DeviceID = NULL;
static cl_uint NumDevice;
static cl_context Context;
static cl_command_queue CommandQueue = NULL;
static cl_program Program = NULL;
static cl_kernel KernelNor = NULL;
static cl_kernel KernelMul = NULL;
static cl_kernel KernelCro = NULL;
static cl_kernel KernelTex = NULL;
static cl_float4* Vector;
static cl_float4* Matrix;
static cl_float4* Result;
static cl_mem RBuffers[4];
static cl_mem WBuffers[4];
static cl_mem imageR;
static cl_mem imageW;
static cl_image_format format;
static unsigned int height, width;
static byte_t *input, *output;

cl_float4* SetVectorSize(int size)
{
	return (cl_float4*)malloc(size * sizeof(cl_float4));
}
cl_float4* SetMatrix4Size(int size)
{
	return (cl_float4*)malloc(size * sizeof(cl_float4) * 4);
}
GLubyte* LoadBmp(const char* imagePath, int* width, int* height)
{
	// Data read from the header of the BMP file
	unsigned char header[54]; // Each BMP file begins by a 54-bytes header
	unsigned int dataPos;     // Position in the file where the actual data begins
	unsigned int imageSize;   // = width*height*3
	// Actual RGB data
	unsigned char* data;

	FILE* file = fopen(imagePath, "rb");
	if (!file) { printf("Image could not be opened\n"); return NULL; }


	if (fread(header, 1, 54, file) != 54) { // If not 54 bytes read : problem
		printf("Not a correct BMP file\n");
		return 0;
	}

	if (header[0] != 'B' || header[1] != 'M') {
		printf("Not a correct BMP file\n");
		return 0;
	}

	// 바이트 배열에서 int 변수를 읽습니다. 
	dataPos = *(int*)&(header[0x0A]);
	imageSize = *(int*)&(header[0x22]);
	*width = *(int*)&(header[0x12]);
	*height = *(int*)&(header[0x16]);

	if (imageSize == 0)    imageSize = *width * *height * 3; // 3 : one byte for each Red, Green and Blue component
	if (dataPos == 0)      dataPos = 54; // The BMP header is done that way

	data = (GLubyte*)malloc(sizeof(GLubyte) * imageSize);

	// 파일에서 버퍼로 실제 데이터 넣기. 
	fread(data, 1, imageSize, file); // bgr

	//이제 모두 메모리 안에 있으니까, 파일을 닫습니다. 
	//Everything is in memory now, the file can be closed
	fclose(file);
	return data;
}

void Setup(void)
{
	cl_int status;
	FILE* fp;
	OCLERROR_RET(
		clGetPlatformIDs(0, NULL, &NumPlatform),
		status,
		FAIL_PLATFORM
	);
	PlatformID = malloc(NumPlatform * sizeof(cl_platform_id));
	if (PlatformID == NULL)
	{
		return -1;
	}
	OCLERROR_RET(
		clGetPlatformIDs(NumPlatform, PlatformID, NULL),
		status,
		FAIL_PLATFORM
	);

	OCLERROR_RET(
		clGetDeviceIDs(*PlatformID, CL_DEVICE_TYPE_GPU, 0, NULL, &NumDevice),
		status,
		FAIL_DEVICE
	);
	DeviceID = malloc(NumDevice * sizeof(cl_device_id));
	if (DeviceID == NULL)
	{
		free(PlatformID);
		return -1;
	}
	OCLERROR_RET(
		clGetDeviceIDs(*PlatformID, CL_DEVICE_TYPE_GPU, NumDevice, DeviceID, NULL),
		status,
		FAIL_DEVICE
	);

	fp = fopen("test.cl", "r");
	if (!fp)
	{
		fprintf(stderr, "Failed to load kernel.\n");
		exit(1);
	}
	char* sourceStr = (char*)malloc(MAX_SOURCE_SIZE);
	size_t sourceSize = fread(sourceStr, 1, MAX_SOURCE_SIZE, fp);
	if (sourceSize == NULL)
	{
		return -1;
	}
	fclose(fp);

	cl_context_properties contextProperties[] =
	{
		CL_CONTEXT_PLATFORM,
		(cl_context_properties)PlatformID[0],
		0
	};

	Context = clCreateContext(contextProperties, NumDevice, DeviceID, NULL, NULL, &status);
	clGetContextInfo(Context, CL_CONTEXT_DEVICES, sizeof(DeviceID), DeviceID, NULL);

	CommandQueue = clCreateCommandQueue(Context, DeviceID[0], 0, &status);

	OCLERROR_PAR(
		Program = clCreateProgramWithSource(Context, 1, (const char**)&sourceStr, &sourceSize, &status),
		status,
		FAIL_PROGRAM
	);
	status = clBuildProgram(Program, 0, NULL, NULL, NULL, NULL);
	if (status != CL_SUCCESS)
	{
		size_t size;

		status = clGetProgramBuildInfo(Program, DeviceID[0], CL_PROGRAM_BUILD_LOG, 0, NULL, &size);
		char* Buildlog = malloc(size);
		status = clGetProgramBuildInfo(Program, DeviceID[0], CL_PROGRAM_BUILD_LOG, size, Buildlog, NULL);
		printf("%s", Buildlog);
		free(Buildlog);
		exit(0);
	}

	KernelNor = clCreateKernel(Program, "Normalization", &status);
	KernelMul = clCreateKernel(Program, "Cross", &status);
	KernelMul = clCreateKernel(Program, "MulMatrix", &status);
	KernelTex = clCreateKernel(Program, "Texture", &status);

	RBuffers[0] = clCreateBuffer(Context, CL_MEM_READ_ONLY, MAX_CL_BUFFER_SIZE, NULL, &status);
	RBuffers[1] = clCreateBuffer(Context, CL_MEM_READ_ONLY, MAX_CL_BUFFER_SIZE, NULL, &status);
	RBuffers[2] = clCreateBuffer(Context, CL_MEM_READ_ONLY, MAX_CL_BUFFER_SIZE, NULL, &status);
	RBuffers[3] = clCreateBuffer(Context, CL_MEM_READ_ONLY, MAX_CL_BUFFER_SIZE, NULL, &status);
	WBuffers[0] = clCreateBuffer(Context, CL_MEM_WRITE_ONLY, MAX_CL_BUFFER_SIZE, NULL, &status);
	WBuffers[1] = clCreateBuffer(Context, CL_MEM_WRITE_ONLY, MAX_CL_BUFFER_SIZE, NULL, &status);
	WBuffers[2] = clCreateBuffer(Context, CL_MEM_WRITE_ONLY, MAX_CL_BUFFER_SIZE, NULL, &status);
	WBuffers[3] = clCreateBuffer(Context, CL_MEM_WRITE_ONLY, MAX_CL_BUFFER_SIZE, NULL, &status);
	
	format.image_channel_order = CL_RGB;
	format.image_channel_data_type = CL_UNSIGNED_INT8;
	
	byte_t data = LoadBmp("../resource/block_texture.bmp", width, height);
	imageR = clCreateImage(Context, CL_MEM_READ_ONLY, &format, width, height, 0,data, NULL );
	imageW = clCreateImage(Context, CL_MEM_WRITE_ONLY, &format, width, height, 0, data, NULL);
	
	for (int i = 0; i < 4; i++)
	{
		MainB[i].Buffer = RBuffers[i];
		MainB[i + 4].Buffer = WBuffers[i];
	}

	for (int i = 0; i < 8; i++)
	{
		SubBuffers[i].bActivated[i] = 0;
		MainB[i].bActivated[i] = 0;
	}

FAIL_PLATFORM:
	return 0;
FAIL_CONTEXTINFO:
FAIL_GETDEVICEID:
	free(DeviceID);
FAIL_COMMANDQUEUE:
	clFlush(CommandQueue);
	clFinish(CommandQueue);
	clReleaseCommandQueue(CommandQueue);
FAIL_PROGRAM:
	clReleaseProgram(Program);
FAIL_DEVICE:
	clReleaseDevice(DeviceID);
}
void Shutdown(void)
{
	clFlush(CommandQueue);
	clFinish(CommandQueue);
	clReleaseKernel(KernelTex);
	clReleaseKernel(KernelNor);
	clReleaseKernel(KernelMul);
	clReleaseKernel(KernelCro);
	clReleaseProgram(Program);
	clReleaseCommandQueue(CommandQueue);
	clReleaseContext(Context);
}


void Normalization(cl_mem* wb, cl_mem* rb, int length)
{
	size_t offset = 0, globalSize = 0, localSize = 0;

	clSetKernelArg(KernelNor, 0, sizeof(cl_mem), &wb);
	clSetKernelArg(KernelNor, 1, sizeof(cl_mem), &rb);
	clEnqueueNDRangeKernel(
		CommandQueue,
		KernelNor,
		1,
		&offset,
		&globalSize,
		&localSize,
		0,
		NULL,
		NULL
	);
	clEnqueueReadBuffer(CommandQueue, );
}
void Cross(cl_mem* wb, int length, int figure, cl_mem* rb)
{
	size_t offset = 0, globalSize = 0, localSize = 0;
	
	clSetKernelArg(KernelCro, 0, sizeof(cl_mem), &wb);
	clSetKernelArg(KernelCro, 1, sizeof(int), &figure);
	clSetKernelArg(KernelCro, 2, sizeof(cl_mem), *rb);
	clEnqueueNDRangeKernel(
		CommandQueue,
		KernelCro,
		1,
		&offset,
		&globalSize,
		&localSize,
		0,
		NULL,
		NULL
	);
}
void MulMatirx(cl_mem* wb1, int vlen, cl_mem* wb2, int mlen, cl_mem* rb)
{
	// 글로벌 사이즈랑 로컬 사이즈 정해줘야 함.
	size_t offset = 0, globalSize = 0, localSize = 0;
	
	clSetKernelArg(KernelMul, 0, sizeof(cl_mem), &wb1);
	clSetKernelArg(KernelMul, 1, sizeof(cl_mem), &wb2);
	clSetKernelArg(KernelMul, 2, sizeof(cl_mem), *rb);
	clEnqueueNDRangeKernel(
		CommandQueue,
		KernelMul,
		1,
		&offset,
		&globalSize,
		&localSize,
		0,
		NULL,
		NULL
	);
}
// 4 * 1벡터에 4 * 4행렬이 들어옴. 연산결과는

void Texture(int* height, int* width)
{
	size_t origin[] = { 0, 0 ,0};
	size_t region[] = { width, height, 1 };
	clEnqueueWriteImage(CommandQueue, imageW, CL_TRUE, &origin, &region, 0, NULL, &input, NULL, NULL, NULL);
	size_t offset = 0, globalSize = 0, localSize = 0;
	clSetKernelArg(KernelTex, 0, sizeof(int), &height);
	clSetKernelArg(KernelTex, 1, sizeof(int), &width);
	clSetKernelArg(KernelTex, 2, sizeof(cl_mem), &imageR);
	clSetKernelArg(KernelTex, 3, sizeof(cl_mem), &imageW);
	globalSize = 1;
	localSize = 1;
	clEnqueueNDRangeKernel(CommandQueue, KernelTex, 1, &offset, &globalSize, &localSize, 0, NULL, NULL);
	clEnqueueReadImage(CommandQueue, imageR, CL_TRUE, &origin, &region, 0, NULL, &output, NULL, NULL, NULL);
}

int main(void)
{
	Setup();
	
	Shutdown();
	return 0;
}