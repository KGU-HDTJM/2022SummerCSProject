#pragma warning (disable:4996)
#include <CL/cl.h>
#include <CL/Utils/Error.h>
#include<stdio.h>
#include<stdlib.h>

#include "HDTJMType.h"
#include "HDTJMDef.h"
#include "HDTJMThread.h"
#include "HeapAlloc.h"
#include "StackAlloc.h"
#include "LinearAlgebra.h"
#define GPGPU_EXPORTS
#include "CLShader.h"
#define KERNEL_FILE_NAME "shader.cl"
#define MEM_BLOCK_SIZE (1 << 16)
#define MAX_BUFFER 1<<16

static cl_platform_id	Platform;
static cl_device_id		Device;
static cl_context		Context;
static cl_command_queue	CmdQueue;
static cl_program		Program;
static size_t			KernelCount;
static size_t			BufferCount;

static eClDeviceType	DeviceType;

static cl_mem* MemBuffer;
static cl_kernel* Kernels;
static cl_uint			ComputeUnitCount;
static cl_uint          MaxWorkItemDim;
static cl_uint* MaxWorkItemSizes;
static size_t			MaxClMemSize;
static size_t			MaxWorkGroup;
static size_t			TextrueSize;

//MAX_SOUCRE_SIZE 오류있음 이거 고칠것, 그리고 파일 링크 시키는거하기
int InitCL(void* textureBuffer, size_t textureWidth, size_t textureHeight, size_t texturePixelSize)
{
	size_t errRet = 0;
	FILE* fp = 0;
	TextrueSize = textureWidth * textureHeight * texturePixelSize;

	if (clGetPlatformIDs(1, &Platform, NULL) < 0)
	{
			printf("Fail clGetPlatformIDs(%s: %d)\n", __FILE__, __LINE__);
			return -1;
	}

	if (clGetDeviceIDs(Platform, CL_DEVICE_TYPE_GPU, 1, &Device, NULL) < 0)
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
	// Get Device Info
	{
		errRet = clGetDeviceInfo(Device, CL_DEVICE_MAX_MEM_ALLOC_SIZE, sizeof(size_t), &MaxClMemSize, NULL);
		errRet = clGetDeviceInfo(Device, CL_DEVICE_MAX_WORK_ITEM_DIMENSIONS, sizeof(size_t), &MaxWorkItemDim, NULL);
		MaxWorkItemSizes = HAlloc(sizeof(size_t) * MaxWorkItemDim, False, NULL);
		errRet = clGetDeviceInfo(Device, CL_DEVICE_MAX_WORK_ITEM_SIZES, sizeof(size_t) * MaxWorkItemDim, MaxWorkItemSizes, NULL);
		errRet = clGetDeviceInfo(Device, CL_DEVICE_MAX_WORK_GROUP_SIZE, sizeof(size_t), &MaxWorkGroup, NULL);
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
	
	fp = fopen(KERNEL_FILE_NAME, "r");
	fseek(fp, 0, SEEK_END);
	size_t kernelSourceSize = ftell(fp) + 1;
	rewind(fp);
	char* sourceStr = HAlloc(kernelSourceSize, False, NULL);
	Program = clCreateProgramWithSource(Context, 1, &sourceStr, &kernelSourceSize, &errRet);
	HFree(sourceStr);
	errRet = clBuildProgram(Program, 1, &Device, NULL, NULL, NULL);
	
	if (errRet != CL_SUCCESS)
	{
		size_t size;
		char* log = HAlloc(size, False, NULL);

		errRet = clGetProgramBuildInfo(Program, Device, CL_PROGRAM_BUILD_LOG, 0, NULL, &size);
		errRet = clGetProgramBuildInfo(Program, Device, CL_PROGRAM_BUILD_LOG, size, log, NULL);

		printf("%s", log);
		HFree(log);
		return -1;
	}

	Kernels = HAlloc(sizeof(cl_kernel) * 7, False, NULL); // 이코드 이해 안감 머선 코드인지 모르겠음... 
	Kernels[0] = clCreateKernel(Program, "ConvertVoxelFromGrid", &errRet);
	Kernels[1] = clCreateKernel(Program, "TransVoxel", &errRet);
	Kernels[2] = clCreateKernel(Program, "Projection", &errRet);
	Kernels[3] = clCreateKernel(Program, "FaceMapping", &errRet);
	Kernels[4] = clCreateKernel(Program, "GetNormal", &errRet);
	Kernels[5] = clCreateKernel(Program, "matrix_mult", &errRet);
	Kernels[6] = clCreateKernel(Program, "transpose", &errRet);

	// VoxelPositionArray buffer
	MemBuffer[0] = clCreateBuffer(Context, CL_MEM_READ_ONLY, MEM_BLOCK_SIZE, NULL, &errRet);
	// Texture Source
	MemBuffer[1] = clCreateBuffer(Context, CL_MEM_READ_ONLY, TextrueSize, NULL, &errRet);
	// PolygonArray
	MemBuffer[2] = clCreateBuffer(Context, CL_MEM_READ_WRITE, MEM_BLOCK_SIZE, NULL, &errRet);
	MemBuffer[3] = clCreateBuffer(Context, CL_MEM_READ_WRITE, MEM_BLOCK_SIZE, NULL, &errRet);
	MemBuffer[4] = clCreateBuffer(Context, CL_MEM_READ_WRITE, MEM_BLOCK_SIZE, NULL, &errRet);
	MemBuffer[5] = clCreateBuffer(Context, CL_MEM_WRITE_ONLY, MEM_BLOCK_SIZE, NULL, &errRet);

	// Texture Output
	MemBuffer[6] = clCreateBuffer(Context, CL_MEM_WRITE_ONLY, TextrueSize * 3, NULL, &errRet);
}


size_t ClVertexShader(Vector3f_t* outPolygonArr, VoxelPos_t* voxelPosArr, size_t length)
{
	size_t gItem, lItem;

	clEnqueueWriteBuffer(CmdQueue, MemBuffer[5], CL_TRUE, 1, sizeof(VoxelPos_t) * length, voxelPosArr, NULL, NULL, NULL);
	clSetKernelArg(Kernels[0], 0, sizeof(cl_mem), MemBuffer[2]);
	clSetKernelArg(Kernels[0], 1,sizeof(cl_mem), MemBuffer[5]);
	clEnqueueNDRangeKernel(CmdQueue, Kernels[0], 1, NULL, &gItem, &lItem, 0, NULL, NULL);
	clEnqueueReadBuffer(CmdQueue, MemBuffer[2], CL_TRUE, 1, sizeof(Vector3f_t) * 36 * length, outPolygonArr, NULL, NULL, NULL);
	
	return;
}

void PixelShader(void* outTexture, Light_t* lightArr, size_t lightCount)
{
	byte_t resultLen;

	clEnqueueWriteBuffer(CmdQueue, MemBuffer[5], CL_TRUE, 1, sizeof(VoxelPos_t) * length, voxelPosArr, NULL, NULL, NULL);
	clSetKernelArg(Kernels[0], 0, sizeof(cl_mem), MemBuffer[2]);
	clSetKernelArg(Kernels[0], 1, sizeof(cl_mem), MemBuffer[5]);
	clEnqueueNDRangeKernel(CmdQueue, Kernels[0], 1, NULL, &gItem, &lItem, 0, NULL, NULL);
	clEnqueueReadBuffer(CmdQueue, MemBuffer[2], CL_TRUE, 1, sizeof(Vector3f_t) * 36 * length, outPolygonArr, NULL, NULL, NULL);
}

void EndCL(void)
{
	clFlush(CmdQueue);
	clFinish(CmdQueue);
	clReleaseKernel(Kernels);
	clReleaseMemObject(MemBuffer);
	clReleaseProgram(Program);
	clReleaseCommandQueue(CmdQueue);
	clReleaseContext(Context);
}


int main(void)
{
	InitCL();

	EndCL();
}