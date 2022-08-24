__kernel void RedShader(__global float4* pixel, int width, int height) 
{
    int i = get_global_id(0);
    int j = width * get_global_id(1);
    int k = width * height * (get_global_id(2) + 1);

    pixel[i + j + k].x = 1.0;
    pixel[i + j + k].y = 0.0;
    pixel[i + j + k].z = 0.0;
}
