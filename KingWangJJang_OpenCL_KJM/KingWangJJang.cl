__constant sampler_t sampler = CLK_NORMALIZED_COORDS_TRUE |
                             CLK_ADDRESS_NONE |
                             CLK_FILTER_NEAREST;

__kerenl void Normalization2(__global float4* Vec, __global float4* Result)
{
	int row = get_global_id(0);
	Result[row] = normalize(float4 Vec);
}

__kernel void Cross(__global float4* Vertext, int Figure, __global float4* Result)
{
	int row = get_global_id(0);
	int Figcount = row * Figure;
	
	float4 reVer1 = Vertext[Figcount + 0] - Vertext[Figcount + 1];
	float4 reVer2 = Vertext[Figcount + 2] - Vertext[Figcount + 1];

	Result[row] = cross(float4 reVer1, float4 reVer2);
}

__kernel void MulMatrix(__global float4* Vec, __global float4* Mat, __global float4* Result)
{
	int row = get_global_id(0);
	int col = get_global_id(1);

	Result[row] = Vec[row] * Mat[col][row];
}

__kernel void Texture(
	__global int* height,
	__global int* width,
	read_only image2d_t src_image,
	write_only image2d_t dst_image
)
{
	for(int i = 0; i < height / 4; i++)
	{
		int aligne = height/4;
		float2 coord = (float2)(get_global_id(0), i);
		float4 clr = read_imagef(src_image, samoler, coord);
		 float4 pixelRed = (float4)(1, 0, 0, 0); 
		 float4 pixelBlue = (float4)(0, 1, 0, 0);
		 float4 pixelGreen = (float4)(0, 0, 1, 0);
		write_only(dst_image, coord, pixelRed);
		write_only(dst_image, coord, pixelBlue);
		write_only(dst_image, coord, pixelGreen);
	}
}

