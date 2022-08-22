#define _CRT_SECURE_NO_WORNINGS
#include <stdlib.h>
#include <CL/cl.h>
#include <CL/Utils/Error.h>
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
typedef struct
{
	cl_mem MemObj;
	byte_t bActivated[MAX_CL_SUB_BUFFER];
} ClMemMangeData_t;

typedef struct
{
	cl_mem Buffer;
	cl_buffer_region Regions;
	byte_t bActivated[MAX_CL_SUB_BUFFER];
}MainBuffer;

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
static cl_kernel KernelTest = NULL;
static cl_float4* Vector;
static cl_float4* Matrix;
static cl_float4* Result;        
static cl_mem RBuffers[4];
static cl_mem WBuffers[4];
static ClMemMangeData_t SubBuffers[8];
static MainBuffer MainB[8];

cl_float4* SetVectorSize(int size)
{
	return (cl_float4*)malloc(size * sizeof(cl_float4));
}
cl_float4* SetMatrix4Size(int size)
{
	return (cl_float4*)malloc(size * sizeof(cl_float4) * 4);
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

	cl_uint maxbuffersize = 0; 
	clGetDeviceInfo(DeviceID, CL_DEVICE_MAX_MEM_ALLOC_SIZE, NULL, NULL, &maxbuffersize);
	printf("%u", maxbuffersize);

	fp = fopen("KingWnagjjangKernel.cl", "r");
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

	RBuffers[0] = clCreateBuffer(Context, CL_MEM_READ_ONLY, MAX_CL_BUFFER_SIZE, NULL, &status);
	RBuffers[1] = clCreateBuffer(Context, CL_MEM_READ_ONLY, MAX_CL_BUFFER_SIZE, NULL, &status);
	RBuffers[2] = clCreateBuffer(Context, CL_MEM_READ_ONLY, MAX_CL_BUFFER_SIZE, NULL, &status);
	RBuffers[3] = clCreateBuffer(Context, CL_MEM_READ_ONLY, MAX_CL_BUFFER_SIZE, NULL, &status);
	WBuffers[0] = clCreateBuffer(Context, CL_MEM_WRITE_ONLY, MAX_CL_BUFFER_SIZE, NULL, &status);
	WBuffers[1] = clCreateBuffer(Context, CL_MEM_WRITE_ONLY, MAX_CL_BUFFER_SIZE, NULL, &status);
	WBuffers[2] = clCreateBuffer(Context, CL_MEM_WRITE_ONLY, MAX_CL_BUFFER_SIZE, NULL, &status);
	WBuffers[3] = clCreateBuffer(Context, CL_MEM_WRITE_ONLY, MAX_CL_BUFFER_SIZE, NULL, &status);

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
	clReleaseKernel(KernelTest);
	clReleaseKernel(KernelNor);
	clReleaseKernel(KernelMul);
	clReleaseKernel(KernelCro);
	clReleaseProgram(Program);
	clReleaseCommandQueue(CommandQueue);
	clReleaseContext(Context);
}
int *BufferIdentification(cl_mem* buffer, eCLBufferType_t type)
{
	cl_mem* buf;
	int key = 0;
	if (type == CLReadBuf) {
		buf = RBuffers;
	}
	else
	{
		buf = WBuffers;
	}
	for (int i = 0; i < 4; i++)
	{
		if (buffer == buf[i])
		{
			if (type == CLWriteBuf)
			{
				key = i + 4;
				break;
			}
			key = i + 1;
			break;
		}
	}
	return key;
}
int *BufferActivateCheck(cl_mem* buffer, eCLBufferType_t type)
{
	int key = BufferIdentification(buffer, type);
	int ret = 0, num = 0;

	num += MainB[key].bActivated[0] * 2 * 2 * 2;
	num += MainB[key].bActivated[1] * 2 * 2;
	num += MainB[key].bActivated[2] * 2;
	num += MainB[key].bActivated[3] * 1;

	for (int i = 0; i < 15; i++)
	{
		if (num & i == num)
		{
			ret = i;
			break;
		}
	}
	return ret;
}
cl_mem* SubBufferAlloc(cl_mem* buffer, size_t size, eCLBufferType_t type)
{
	int subStateTable[] = { 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0, -1 };
	int subMaxSize[] = { 4, 3, 2, 3, 2, 1, 1, 1, 3, 2, 1, 1, 2, 1 ,1, 0 };

	int alingedSize = SUB_UNIT;
	int key1 = BufferIdentification(buffer, type);
	int key2 = BufferActivateCheck(buffer, type);
	int max = subMaxSize[key1] * sizeof(cl_float4) * SUB_UNIT;

	MainB[key1].Regions.origin = subStateTable[key1] * sizeof(cl_float4) * SUB_UNIT;

	for (int i = 0; alingedSize > size; i++) 
	{ 
		alingedSize *= 2; 
	}
	if (alingedSize > max || subStateTable[key1] == -1)
	{
		if (MainB[key1 + 1].Buffer == RBuffers[4] || MainB[key1 + 1].Buffer == WBuffers[4])
		{
			printf("OUT OF RESOURCE YOU ASSHOLE");
			exit(0);
		}
		SubBufferAlloc(size, MainB[key1 + 1].Buffer, type);
	}
	
	MainB[key1].Regions.size = alingedSize;
	
	for (int i = 3, j = 1; SUB_UNIT * j <= alingedSize; i--, j++)
	{
		MainB[key1].bActivated[i] = 1;
		SubBuffers[key1].bActivated[i] = 1;
	}

	if (type == CLReadBuf)
	{
		return clCreateSubBuffer(
			MainB[key1].Buffer,
			CL_MEM_READ_ONLY,
			CL_BUFFER_CREATE_TYPE_REGION,
			&MainB[key1].Regions,
			NULL
		);
	}
	else
	{
		return clCreateSubBuffer(
			MainB[key1].Buffer,
			CL_MEM_WRITE_ONLY,
			CL_BUFFER_CREATE_TYPE_REGION,
			&MainB[key1].Regions,
			NULL
		);
	}
}
void SubBufferFree(cl_mem* buffer, eCLBufferType_t type)
{
	int key = BufferIdentification(buffer, type);
	for (int i = 0; i < 4; i++)
	{
		if (SubBuffers[key].bActivated[i] == 1)
		{
			MainB[key].bActivated[i] = 0;
		}
	}
}

void Normalization(cl_mem* wb, cl_mem* rb, int length)
{
	// �۷ι� ������� ���� ������ ������� ��.
	size_t offset = 0, globalSize = 0, localSize = 0;
	clSetKernelArg(KernelNor, 0, sizeof(cl_mem), &wb);
	/*clSetKernelArg(KernelNor, 1, sizeof(cl_float4) * length, NULL);
	clSetKernelArg(KernelNor, 1, sizeof(int), length);*/
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
}
void Cross(cl_mem* wb, int length, int figure, cl_mem* rb)
{
	// �۷ι� ������� ���� ������ ������� ��.
	size_t offset = 0, globalSize = 0, localSize = 0;
	clSetKernelArg(KernelCro, 0, sizeof(cl_mem), &wb);
	//clSetKernelArg(KernelCro, 1, sizeof(cl_float4) * length, NULL);
	//clSetKernelArg(KernelCro, 1, sizeof(int), &length);
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
	// �۷ι� ������� ���� ������ ������� ��.
	size_t offset = 0, globalSize = 0, localSize = 0;
	clSetKernelArg(KernelMul, 0, sizeof(cl_mem), &wb1);
	clSetKernelArg(KernelMul, 1, sizeof(cl_mem), &wb2);
	/*clSetKernelArg(KernelNor, 2, sizeof(cl_float4) * vlen, NULL);
	clSetKernelArg(KernelNor, 2, sizeof(int),vlen);
	clSetKernelArg(KernelNor, 3, sizeof(cl_float4) * mlen, NULL);
	clSetKernelArg(KernelNor, 3, sizeof(int) ,mlen);*/
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
// 4 * 1���Ϳ� 4 * 4����� ����. ��������

int main(void)
{
	Setup();
	
	Shutdown();
	return 0;
}