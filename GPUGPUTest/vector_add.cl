__kernel void VectorAdd(
    __global int* A,
    __global int* B,
    __global int* C )
{
    int idx = get_global_id(0);
    C[idx] = A[idx] + B[idx];
}

// __kernel void VectorSub(
//     __global int* A,
//     __global int* B,
//     __global int* C )
// {
//     int idx = get_global_id(0);
//     C[idx] = A[idx] + B[idx];
// }
