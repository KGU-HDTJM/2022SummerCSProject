// https://youtu.be/bZ-66xjMx9s

// voxelPositionArr(cpu) ->(gpu->) Voxel * Matrix * ProjectionMatrix 이 결과를 폴리곤 array로 바꿔야 한다.
// -> polygon array ->폴리곤 개수만큼 Normal구하고 -> texture = 노말이랑 lightSource배열을 곱함  => result texture(cpu)
//                  => uv coordinate(cpu)

// [x][y][z][tex_id]
#define VOXEL_SIZE 1.F
#define MAX_FACE_COUNT 3
#define TEXTURE_WIDTH
#define TEXTURE_HEIGHT
#define TEXTURE_COUNT

__constant float3 VOXEL_VERTEX_SAMPLE[8] = 
{
    {-1.F,-1.F, 1.F},
    {-1.F, 1.F,-1.F},
    {-1.F,-1.F,-1.F},
    {-1.F, 1.F, 1.F},
    { 1.F,-1.F,-1.F},
    { 1.F,-1.F, 1.F},
    { 1.F, 1.F,-1.F},
    { 1.F, 1.F, 1.F}
};

__constant int3 VOXEL_FACE_INDEX[36] = 
{
    6,2,0,
    0,4,6,

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
    __global float3* outPolygonArr,
    __global float2* outTextureCoord,
    __global float3* outNormalArr,
    __global char4* voxelPositionArr,
    __global float16 transMatrix,
    __global int3 gridSize,
    __global int *faceCount
)// 그리드는 위치이다. 위치를 구하는 함수를 사용할것?
{
    int idx = get_global_id(0);
    float4 voxel[8];
    float4 temp;
    float3 polygon[18];
    float2 texCoord[18]
    int visibleFaceCount = 0;
    int i;
    int j;
    float textrueID = (float)voxelPositionArr[idx].w;
    float2 texPos = (float2){0.F,0.F};
    texPos.x = textureID / TEXTURE_COUNT;
    const char3 gridHalfSize = gridSize / 2;
    // voxelPosition -> VoxelVertex
    for (i = 0; i < 8; i++)
    {
        temp.xyz = VOXEL_VERTEX_SAMPLE[i] + convert_float3(voxelPosition[idx].xyz - gridhalfSize);
        temp.w = 1.F;
        //voxel[i] = (float4){ 0.F, 0.F, 0.F, 0.F };
        // vector * matrix
        voxel[i].x = temp.x * (matrx.0 + transMatrix.4 + transMatrix.8 + transMatrix.c);
        voxel[i].y = temp.y * (matrx.1 + transMatrix.5 + transMatrix.9 + transMatrix.d);
        voxel[i].z = temp.z * (matrx.2 + transMatrix.6 + transMatrix.a + transMatrix.e);
        voxel[i].w = temp.w * (matrx.3 + transMatrix.7 + transMatrix.b + transMatrix.f); 
    }
    float3 v0;
    float3 v1;
    float3 normal[6];
    for (i = 0; i < 6; i++)
    {
        for (j = 0; j < 6; j++)
        {
            polygon[6 * visibleFaceCount + j] = voxel[VXOEL_FACE_INDEX[6 * i + j]].xyz;
            texCoord[6 * visibleFaceCount + j].x = (GetTextureCoord(j).x / TEXTURE_COUNT);
            texCoord[6 * visibleFaceCount + j].y = (GetTextureCoord(j).y / 3);
            texCoord[6 * visibleFaceCount + j] += texPos;
        }
        v0 = voxel[VOXEL_FACE_INDEX[6 * i]].xyz - voxel[VOXEL_FACE_INDEX[6 * i + 1]].xyz;
        v1 = voxel[VOXEL_FACE_INDEX[6 * i]].xyz - voxel[VOXEL_FACE_INDEX[6 * i + 2]].xyz;
        normal[visibleFaceCount] = normalize(cross(v0, v1));
        visibleFaceCount += 0.F < normal[i].z;
    }
    for (i = 0; i < visibleFaceCount; i++)
    {
        for (j = 0; j < 6; j++)
        {
            outPolygonArr[6 * idx * visibleFaceCount + j] = polygon[6 * i + j];
            outTextureCoord[6 * idx * visibleFaceCount + j] = texCoord[6 * i + j];
            outNormalArr[6 * visibleFaceCount * idx + j] = normal[6 * i + j];
        }
    }
    *faceCount = visibleFaceCount;
}

//offset = 1차원, sizeof(cl_float16) * length(tranMatrix)만큼하면 됨
// outVoxel = ReadWritebuffer. 

typedef struct
{
    float3 Position;
    char4 RGBA;
} LightSource_t;

// work group 
__kernel void PixelShader(
    __global char3* outPixel,
    __global float3* normal,
    __global float3 view,
    __global LightSource_t* lightArr,
      int lightLength, 
    __global char3* textureSource
)
{
    size_t x = get_global_id(0);
    size_t y = get_global_id(1);
    size_t normalIdx = get_global_id(2);
    float3 pixel;

    pixel = (contvert_float3(textureSource[TEXTURE_WIDTH * y + x]) / 255.F);

    float3 halfVector = (normalize(lightArr.Position) + normalize(view)) / 2;
    float brightness = dot(normal[normalIdx], halfVector);
    outPixel[TEXTURE_WIDTH * y + x + normalIdx * TEXTURE_WIDTH * TEXTURE_HEIGHT] = convert_char3(pixel * brightness * 255.F);
}

__kernel void TextureMapping(
    __global float2* textureCoord,
    __global float3* normal,
    __global float3* faceID, 
    int length
)
{
   int condition = 0; 
   int row = get_global_id(0);
   for(int i = 0; i < length; i++)
   {
     condition |= (0.8 < dot(faceID[row], normal[row]))<<i;
   }
   textureCoord[row].y += (1.0F/3.F) * (condition >> 1); 
}
