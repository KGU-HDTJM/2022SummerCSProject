__kernel void Add(__global int* a, __global int* b, __global int* result)
{
	int row = get_global_id(0);

	result[row] = a[row] + b[row];
}