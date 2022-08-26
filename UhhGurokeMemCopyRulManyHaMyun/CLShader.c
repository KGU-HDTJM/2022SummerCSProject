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
#define MAX_BUFFER (1<<16)
#define KERNEL_COUNT 3
#define MEM_BLOCK_COUNT 8

typedef enum
{
	VoxelPositionArray = 0, TextureSource = 1, PolygonArray = 2, NormalBuffer = 5, TextureCoord = 6, TextureResult = 7
} eClMemIdx_t;

static cl_platform_id		Platform;
static cl_device_id		Device;
static cl_context			Context;
static cl_command_queue	CmdQueue;
static cl_program		Program;
static size_t				BufferCount;

static eClDeviceType	DeviceType;

static cl_mem* MemBuffer;
static cl_kernel VertexShader;
static cl_kernel PixelShader;
static cl_kernel TextureMapping;

static cl_uint			ComputeUnitCount;
static cl_uint          MaxWorkItemDim;
static cl_uint* MaxWorkItemSizes;
static size_t			MaxClMemSize;
static size_t			MaxWorkGroup;
static size_t			TextrueSize;
static size_t          TextrueHeight;
static size_t          TextrueWidth;

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
	
	VertexShader = clCreateKernel(Program, "VertexShader", &errRet);
	PixelShader = clCreateKernel(Program, "PixelShader", &errRet);
	TextureMapping = clCreateKernel(Program, "TextureMapping", &errRet);
	MemBuffer = HAlloc(sizeof(cl_mem) * MEM_BLOCK_COUNT, False, NULL);
	// VoxelPositionArray buffer
	MemBuffer[0] = clCreateBuffer(Context, CL_MEM_READ_ONLY, MEM_BLOCK_SIZE, NULL, &errRet);
	// Texture Source
	MemBuffer[1] = clCreateBuffer(Context, CL_MEM_READ_ONLY, TextrueSize, NULL, &errRet);
	// PolygonArray
	MemBuffer[2] = clCreateBuffer(Context, CL_MEM_READ_WRITE, MEM_BLOCK_SIZE, NULL, &errRet);
	MemBuffer[3] = clCreateBuffer(Context, CL_MEM_READ_WRITE, MEM_BLOCK_SIZE, NULL, &errRet);
	MemBuffer[4] = clCreateBuffer(Context, CL_MEM_READ_WRITE, MEM_BLOCK_SIZE, NULL, &errRet);
	// normal buffer
	MemBuffer[5] = clCreateBuffer(Context, CL_MEM_READ_WRITE, MEM_BLOCK_SIZE, NULL, &errRet);
	// texture coord
	MemBuffer[6] = clCreateBuffer(Context, CL_MEM_READ_WRITE, MEM_BLOCK_SIZE, NULL, &errRet);
	// Texture Output
	MemBuffer[7] = clCreateBuffer(Context, CL_MEM_WRITE_ONLY, TextrueSize * 3, NULL, &errRet);
}
//
//size_t ClVertexShader(
//	Vector3f_t* outPolygonArr, 
//	Vector2f_t* outTextureCoord, 
//	Matrix4_t* transMatrix,
//	Vector3i_t* gridSize,
//	VoxelPos_t* voxelPosArr,
//	size_t length)
//{
//	size_t gItem, lItem;
//	clEnqueueWriteBuffer(CmdQueue, MemBuffer[5], CL_TRUE, 1, sizeof(VoxelPos_t) * length, voxelPosArr, NULL, NULL, NULL);
//	clSetKernelArg(Kernels[0], 0, sizeof(cl_mem), MemBuffer[]);
//	clSetKernelArg(Kernels[0], 1,sizeof(cl_mem), MemBuffer[]);
//	clEnqueueNDRangeKernel(CmdQueue, Kernels[0], 1, NULL, &gItem, &lItem, 0, NULL, NULL);
//	clEnqueueReadBuffer(CmdQueue, MemBuffer[2], CL_TRUE, 1, sizeof(Vector3f_t) * 36 * length, outPolygonArr, NULL, NULL, NULL);
//	
//	return;
//}
//
//void ClPixelShader(void* outTexture, cl_mem* normal,  Vector3f_t view, Light_t* lightArr, size_t lightCount)
//{
//	size_t gItem[3];
//	size_t lItem[3];
//
//
//	clEnqueueWriteBuffer(CmdQueue, MemBuffer[5], CL_TRUE, 1, sizeof(VoxelPos_t) * length, voxelPosArr, NULL, NULL, NULL);
//	clSetKernelArg(PixelShader, 0, sizeof(cl_mem), MemBuffer[0]);
//	clSetKernelArg(PixelShader, 1, sizeof(cl_mem), MemBuffer[5]);
//	clEnqueueNDRangeKernel(CmdQueue, PixelShader, 1, NULL, &gItem, &lItem, 0, NULL, NULL);
//	clEnqueueReadBuffer(CmdQueue, MemBuffer[2], CL_TRUE, 1, sizeof(Vector3f_t) * 36 * length, outTexture, NULL, NULL, NULL);
//}
//
//void ClTextureMapping(Vector2f_t* textureCoord, Vector3f_t* normal, size_t length, Vector3f_t* faceID)
//{
//	size_t gItem, lItem;
//
//	clEnqueueWriteBuffer(CmdQueue, MemBuffer[5], CL_TRUE, 1, sizeof(VoxelPos_t) * length, voxelPosArr, NULL, NULL, NULL);
//	clSetKernelArg(Kernels[0], 0, sizeof(cl_mem), MemBuffer[0]);
//	clSetKernelArg(Kernels[0], 1, sizeof(cl_mem), MemBuffer[5]);
//	clEnqueueNDRangeKernel(CmdQueue, Kernels[0], 1, NULL, &gItem, &lItem, 0, NULL, NULL);
//	clEnqueueReadBuffer(CmdQueue, MemBuffer[2], CL_TRUE, 1, sizeof(Vector3f_t) * 36 * length, outPolygonArr, NULL, NULL, NULL);
//}
//

size_t ClVertexShader(Vector3f_t* outPolygonArr,
	Vector2f_t* outTextureCoord, cl_mem* outNormalBuffer,
	VoxelPos_t* voxelPositionArr, size_t length, Matrix4_t* transMatrix, Vector3i_t* gridSize
)
{
	int faceCount; 
	size_t polygonArrLength = length * 3;
	clEnqueueWriteBuffer(CmdQueue, MemBuffer[VoxelPositionArray], CL_TRUE, 0, length, voxelPositionArr, NULL, NULL, NULL);
	clSetKernelArg(VertexShader, 0, sizeof(cl_mem), MemBuffer[PolygonArray]);
	clSetKernelArg(VertexShader, 1, sizeof(cl_mem), MemBuffer[TextureCoord]);
	clSetKernelArg(VertexShader, 2, sizeof(cl_mem), MemBuffer[NormalBuffer]);
	clSetKernelArg(VertexShader, 3, sizeof(cl_mem), MemBuffer[VoxelPositionArray]);
	clSetKernelArg(VertexShader, 4, sizeof(Matrix4_t), &transMatrix);
	clSetKernelArg(VertexShader, 5, sizeof(Vector3i_t), &gridSize);
	clSetKernelArg(VertexShader, 6, sizeof(int), &faceCount);
	clEnqueueNDRangeKernel(CmdQueue, VertexShader, 1,NULL, &length, 1, NULL, NULL, NULL);
	clEnqueueReadBuffer(CmdQueue, MemBuffer[PolygonArray], CL_TRUE, 0, sizeof(int), &faceCount, NULL, NULL, NULL);
	clEnqueueReadBuffer(CmdQueue, MemBuffer[PolygonArray], CL_TRUE, 0, sizeof(Vector3f_t)*polygonArrLength * faceCount , outPolygonArr, NULL, NULL, NULL);
	clEnqueueReadBuffer(CmdQueue, MemBuffer[TextureCoord], CL_TRUE, 0, sizeof(Vector2f_t)* polygonArrLength* faceCount, outTextureCoord , NULL, NULL, NULL);
	
	return faceCount;
}

void ClPixelShader(char* outPixel, int length, Vector3f_t* normal, 
	Vector3f_t* view, Light_t* lightArr, int* lightLength, char* textureSource, size_t faceCount)
{
	size_t global[3] = { TextrueWidth, TextrueHeight, 3 };
	clEnqueueWriteBuffer(CmdQueue, MemBuffer[0], CL_TRUE, 0, sizeof(char) *  length, normal, NULL, NULL, NULL);
	clEnqueueWriteBuffer(CmdQueue, MemBuffer[1], CL_TRUE, 0, sizeof(Vector3f_t), view, NULL, NULL, NULL);
	clEnqueueWriteBuffer(CmdQueue, MemBuffer[2], CL_TRUE, 0, sizeof(Light_t), lightArr, NULL, NULL, NULL);
	clEnqueueWriteBuffer(CmdQueue, MemBuffer[3], CL_TRUE, 0, sizeof(char)* TextrueSize, textureSource, NULL, NULL, NULL);
	clSetKernelArg(PixelShader, 0, sizeof(cl_mem), MemBuffer[0]);
	clSetKernelArg(PixelShader, 1, sizeof(Vector3f_t), &normal);
	clSetKernelArg(PixelShader, 2, sizeof(cl_mem), MemBuffer[1]);
	clSetKernelArg(PixelShader, 3, sizeof(cl_mem), MemBuffer[2]);
	clSetKernelArg(PixelShader, 4, sizeof(int), &lightLength);
	clSetKernelArg(PixelShader, 4, sizeof(cl_mem), MemBuffer[3]);
	clEnqueueNDRangeKernel(CmdQueue, PixelShader, 1,NULL, &global, 1, NULL, NULL, NULL);
	clEnqueueReadBuffer(CmdQueue, MemBuffer[4], CL_TRUE, 0, sizeof(char) * faceCount, outPixel, NULL, NULL, NULL);
}

void ClTextureMapping(Vector2f_t* outTextureCoord, cl_mem* normal, Vector3f_t* normalID, int faceCount)
{
	clEnqueueReadBuffer(CmdQueue, )
}
void EndCL(void)
{
	clFlush(CmdQueue);
	clFinish(CmdQueue);
	clReleaseKernel(VertexShader);
	clReleaseKernel(PixelShader);
	clReleaseKernel(TextureMapping);
	for (size_t i = 0; i < ; i++)
	{

	}
	clReleaseProgram(Program);
	clReleaseCommandQueue(CmdQueue);
	clReleaseContext(Context);
}


int main(void)
{
	InitCL();

	EndCL();
}