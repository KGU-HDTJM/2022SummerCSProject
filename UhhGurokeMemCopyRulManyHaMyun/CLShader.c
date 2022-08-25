#pragma warning (disable:4996)
#include <CL/cl.h>
#include <CL/Utils/Error.h>

#include "HDTJMType.h"
#include "HDTJMDef.h"
#include "HDTJMThread.h"
#include "HeapAlloc.h"
#include "StackAlloc.h"
#define GPGPU_EXPORTS
#include "CLShader.h"
#define KERNEL_FILE_NAME "shader.cl"
#define MAX_BUFFER 1<<16

static cl_platform_id	Platform;
static cl_device_id		Device;
static cl_context		Context;
static cl_command_queue	CmdQueue;
static cl_program		Program;
static size_t			KernelCount;
static size_t			BufferCount;
static cl_uint          MaxWorkItemDim;
static cl_uint			ComputeUnitCount;
static eClDeviceType	DeviceType;
static size_t			MaxClMemSize;
static cl_mem* MemBuffer;
static cl_kernel* Kernels;
static cl_uint* SorkItemSizes;

int InitCL(void)
{
	size_t errRet = 0;
	FILE* fp = 0;
	fp = fopen(KERNEL_FILE_NAME, "r");
	if(!fp)
	{
		fprintf(stderr, "Failed to load kernel.\n");
	}
	char* sourceStr = (char*)malloc(MAX_SOURCE_SIZE);
	size_t sourceSize = fread(sourceStr, 1, MAX_SOURCE_SIZE, fp);
	if (sourceSize == NULL)
	{
		printf("No source in this file\n");
		return -1;
	}
	fclose(fp);

	if (clGetPlatformIDs(1, &Platform, NULL) < 0)
	{
		printf("Fail clGetPlatformIDs(%s: %d)\n", __FILE__, __LINE__);
		return -1;
	}
	errRet = clGetDeviceIDs(Platform, CL_DEVICE_TYPE_GPU, 1, &Device, NULL);
	if (errRet < 0)
	{
		if (clGetDeviceIDs(Platform, CL_DEVICE_TYPE_CPU, 1, &Device, NULL) < 0)
		{
			printf("Fail clGetPlatformIDs(%s: %d)\n", __FILE__, __LINE__);
			return -1;
		}
		else
		{
			printf("Using CPU type device\n");
		}
	}
	
	{
		errRet = clGetDeviceInfo(Device, CL_DEVICE_MAX_MEM_ALLOC_SIZE, sizeof(size_t), &MaxClMemSize, NULL);
		errRet = clGetDeviceInfo(Device, )
	}


	Context = clCreateContext(NULL, 1, &Device, NULL, NULL, &errRet);
	if (errRet < 0)
	{
		printf("Fail Create Context\n");
		return -1;
	}
	CmdQueue = clCreateCommandQueue(Context, Device, 0, &errRet);
	if (errRet < 0)
	{
		printf("Failed Create Command Queue\n");
		return -1;
	}
	Program = clCrearteProgramWithSource(Context, 1, &sourceStr, &sourceSize, &errRet);
	errRet = clBuildProgram(Program, 0, NULL, NULL, NULL, NULL);
	if (errRet != CL_SUCCESS)
	{
		size_t size;
		char* buildlog = malloc(size);

		errRet = clGetProgramBuildInfo(Program, Device, CL_PROGRAM_BUILD_LOG, 0, NULL, &size);
		errRet = clGetProgramBuildInfo(Program, Device, CL_PROGRAM_BUILD_LOG, size, buildlog, NULL);
		
		printf("%s", buildlog);
		free(buildlog);
		return -1;
	}

	Kernels[0] = clCreateKernel(Program, "ConvertVoxelFromGrid", &errRet);
	Kernels[1] = clCreateKernel(Program, "TransVoxel", &errRet);
	Kernels[2] = clCreateKernel(Program, "Projection", &errRet);
	Kernels[3] = clCreateKernel(Program, "FaceMapping", &errRet);
	Kernels[4] = clCreateKernel(Program, "GetNormal", &errRet);
	Kernels[5] = clCreateKernel(Program, "matrix_mult", &errRet);
	Kernels[6] = clCreateKernel(Program, "transpose", &errRet);

	MemBuffer[0] = clCreateBuffer(Context, CL_MEM_READ_ONLY,MAX_BUFFER, &errRet);
	MemBuffer[1] = clCreateBuffer(Context, CL_MEM_READ_ONLY,MAX_BUFFER, &errRet);
	MemBuffer[2] = clCreateBuffer(Context, CL_MEM_READ_ONLY,MAX_BUFFER, &errRet);
	MemBuffer[3] = clCreateBuffer(Context, CL_MEM_READ_WRITE,MAX_BUFFER, &errRet);
	MemBuffer[4] = clCreateBuffer(Context, CL_MEM_READ_WRITE,MAX_BUFFER, &errRet);
	MemBuffer[5] = clCreateBuffer(Context, CL_MEM_WRITE_ONLY,MAX_BUFFER, &errRet);
	MemBuffer[6] = clCreateBuffer(Context, CL_MEM_WRITE_ONLY,MAX_BUFFER, &errRet);
}

void EndCL(void)
{
	clFlush(CommandQueue);
	clFinish(CommandQueue);
	clReleaseKernel(Kernels);
	clReleaseMemObject(MemBuffer);
	clReleaseProgram(Program);
	clReleaseCommandQueue(CmdQueue);
	clReleaseContext(Context);
}

void 

int main(void)
{
	InitCL();

	EndCL();
}