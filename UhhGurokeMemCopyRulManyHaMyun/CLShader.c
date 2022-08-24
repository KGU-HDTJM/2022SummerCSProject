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




}


int main(void)
{
	InitCL();


}