// https://youtu.be/bZ-66xjMx9s

// voxelPositionArr(cpu) ->(gpu->) Voxel * Matrix * ProjectionMatrix 이 결과를 폴리곤 array로 바꿔야 한다.
// -> polygon array ->폴리곤 개수만큼 Normal구하고 -> texture = 노말이랑 lightSource배열을 곱함  => result texture(cpu)
//                  => uv coordinate(cpu)

// [x][y][z][tex_id]
#define VOXEL_SIZE 1.F

__constant float3 VOXEL_VERTEX_SAMPLE[8] = 
{
    {-1.F,-1.F,-1.F},
    {-1.F,-1.F, 1.F},
    {-1.F, 1.F,-1.F},
    {-1.F, 1.F, 1.F},
    { 1.F,-1.F,-1.F},
    { 1.F,-1.F, 1.F},
    { 1.F, 1.F,-1.F},
    { 1.F, 1.F, 1.F}
};

__constant int3 VOXEL_FACE_INDEX[36] = 
{
    6,2,0,
    0,4,6

    3,7,5,
    5,1,3,

    2,6,7,
    7,3,2,

    1,5,4,
    4,0,1,

    7,6,4,
    4,5,7,

    2,3,1,
    1,0,2
};

__constant float2 TEXTURE_COORD_TABLE[8] =
{
    {0.F ,0.F},
	{1.F, 0.F},
	{1.F, 1.F},
	{1.F, 1.F},
	{0.F, 1.F},
	{0.F ,0.F},
    {0.F ,0.F},
	{1.F, 0.F}
};

#define GetTextureCoord(index) TEXTURE_COORD_TABLE[(index + 2 * (index >> 3)) & 7]

// offset =  1차원, sizeof(cl_int4) * length(voxelPositionArr)만큼하면 됨
__kernel void VertexShader(
    __global float4* outVoxel,
    __global char16* voxelPositionArr,
    __global float4x4 )// 그리드는 위치이다. 위치를 구하는 함수를 사용할것?
{
    int idx = 8 * get_global_id(0);
    int col = get_golbal_id(1);
    // voxelPosition -> VoxelVertex 
    // VoxelVertex * Matrix -> resultVertex
    // resultVertex -> PolygonArray
    // polygonArray -> Normal
    // =================result================================ 
    // polygonArray
    // normal
    // texture coordinate

}

//offset = 1차원, sizeof(cl_float16) * length(tranMatrix)만큼하면 됨
// outVoxel = ReadWritebuffer. 
__kernel void MulMatrix(
    __global float4* outVoxel,
    __global float4* inVoxel,
    __global float16* tranMatrix )
{
    int row = get_global_id(0);

    outVoxel[row].x = inVoxel[row].x * (tranMatrix[row].0 + tranMatrix[row].4 + tranMatrix[row].8 + tranMatrix[row].c);
    outVoxel[row].y = inVoxel[row].y * (tranMatrix[row].1 + tranMatrix[row].5 + tranMatrix[row].9 + tranMatrix[row].d);
    outVoxel[row].z = inVoxel[row].z * (tranMatrix[row].2 + tranMatrix[row].6 + tranMatrix[row].a + tranMatrix[row].e);
    outVoxel[row].w = inVoxel[row].w * (tranMatrix[row].3 + tranMatrix[row].7 + tranMatrix[row].b + tranMatrix[row].f);
}
