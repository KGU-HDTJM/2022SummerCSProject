// https://youtu.be/bZ-66xjMx9s



// voxelPositionArr(cpu) ->(gpu->) Voxel * Matrix * ProjectionMatrix 
// -> polygon array -> Normal -> texture => result texture(cpu)
//                  => uv coordinate(cpu)

// [x][y][z][tex_id]
__kernel void ConvertVoxelFromGrid(__global float4* outVoxel, __global int4* voxelPositionArr, int3 girdSize)
{

}

__kernel void TransVoxel(__global float4* outVoxel, __global float4x4* tranMatrix, __global )
{

}

__kernel void ProjectionTrans()
// outPolygon: read write
__kernel void FaceMapping(__global float3* outPolygonArr, __global float3* voxelArr)
{
    
}
// outNormal: read write
__kernel void GetNormal(__global float3* outNormal, __global float3* polygonArr)
{

}




