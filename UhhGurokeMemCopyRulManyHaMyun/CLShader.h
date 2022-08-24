#pragma once
#ifndef CLSHADER_H
#define CLSHADER_H

#if defined(GPGPU_EXPORTS)
#define CL_API extern __declspec(dllexport)
#else
#define CL_API extern __declspec(dllimport)
#endif

typedef enum
{
	DEVICE_TYPE_GPU, DEVICE_TYPE_CPU
} eClDeviceType;

typedef enum
{
	KERNEL_VERTEX_SHADER, KERNEL_PIXEL_SHADER
} eKernelID_t;


CL_API int InitCL(void);
CL_API void ClVertexShader(void);
CL_API void ClPixelShader(void);
CL_API void ReleaseCL(void);

#endif

