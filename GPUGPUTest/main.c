#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#pragma warning(disable: 4996)
#include "CL/cl.h"
#include "HDTJMType.h"
#include "HDTJMDef.h"
#include "StackAlloc.h"
#include "LinearAlgebra.h"

#define KERNEL_SOURCE_BUFFER 0x8000

int main(void)
{
	int* a = NULL;
	int* b = NULL;
	int* c = NULL;

	const int elements = 2048;

	size_t dataSize = sizeof(int) * elements;
	FILE* kernelFile;

	kernelFile = fopen("vector_add.cl", "r");

	char* programSource = malloc(KERNEL_SOURCE_BUFFER);
	size_t soureceLength = fread(programSource, 1, KERNEL_SOURCE_BUFFER, kernelFile);
	fclose(kernelFile);

	a = malloc(dataSize);
	b = malloc(dataSize);
	c = malloc(dataSize);

	for (size_t i = 0; i < elements; i++)
	{
		a[i] = i;
		b[i] = 2 * i;
	}

	cl_int status;

	// Get Platforms
	cl_uint numPlatforms = 0;
	status = clGetPlatformIDs(0, NULL, &numPlatforms);
	cl_platform_id* platforms = NULL;
	platforms = malloc(sizeof(cl_platform_id) * numPlatforms);
	status = clGetPlatformIDs(numPlatforms, platforms, NULL);

	// Get Devices
	cl_uint numDevices = 0;
	status = clGetDeviceIDs(platforms[0], CL_DEVICE_TYPE_ALL, 0, NULL, &numDevices);
	cl_device_id* devices;
	devices = malloc(sizeof(cl_device_id) * numDevices);
	status = clGetDeviceIDs(platforms[0], CL_DEVICE_TYPE_ALL, numDevices, devices, NULL);

	// Context;
	cl_context context;
	context = clCreateContext(NULL, numDevices, devices, NULL, NULL, &status);

	// Command Queue
	cl_command_queue cmdQueue;
	cmdQueue = clCreateCommandQueue(context, devices[0], 0, &status);

	cl_mem bufA;
	bufA = clCreateBuffer(context, CL_MEM_READ_ONLY, dataSize, NULL, &status);
	cl_mem bufB;
	bufB = clCreateBuffer(context, CL_MEM_READ_ONLY, dataSize, NULL, &status);
	cl_mem bufC;
	bufC = clCreateBuffer(context, CL_MEM_WRITE_ONLY, dataSize, NULL, &status);

	status = clEnqueueWriteBuffer(cmdQueue, bufA, CL_FALSE, 0, dataSize, a, 0, NULL, NULL);
	status = clEnqueueWriteBuffer(cmdQueue, bufB, CL_FALSE, 0, dataSize, b, 0, NULL, NULL);

	cl_program program;
	program = clCreateProgramWithSource(context, 1, &programSource, &soureceLength, &status);
	status = clBuildProgram(program, numDevices, devices, NULL, NULL, NULL);

	cl_kernel add;
	//cl_kernel sub;
	add = clCreateKernel(program, "VectorAdd", &status);
	// sub = clCreateKernel(program, "VectorSub", &status);

	status = clSetKernelArg(add, 0, sizeof(cl_mem), &bufA);
	status = clSetKernelArg(add, 1, sizeof(cl_mem), &bufB);
	status = clSetKernelArg(add, 2, sizeof(cl_mem), &bufC);

	size_t globalWorkSize[1] = { elements };
	status = clEnqueueNDRangeKernel(cmdQueue, add, 1, NULL, globalWorkSize, NULL, 0, NULL, NULL);

	// device memory -> host memory
	clEnqueueReadBuffer(cmdQueue, bufC, CL_TRUE, 0, dataSize, c, 0, NULL, NULL);

	for (size_t i = 0; i < elements; i++)
	{
		printf("%d\n", c[i]);
	}

	clReleaseKernel(add);
	//clReleaseKernel(sub);
	clReleaseProgram(program);
	clReleaseCommandQueue(cmdQueue);
	clReleaseMemObject(bufA);
	clReleaseMemObject(bufB);
	clReleaseMemObject(bufC);
	clReleaseContext(context);

	free(a);
	free(b);
	free(c);
	free(platforms);
	free(devices);
	free(programSource);
	return 0;
}