

[shader("compute")]
[numthreads(16,16,1)]
void computeMain(
    uint3 threadId : SV_DispatchThreadID,
    uint3 localThreadId : SV_GroupThreadID,
    uniform RWTexture2D image)
{
    int2 texelCoord = threadId.xy;
    int2 size;
    image.GetDimensions(size.x, size.y);

    if(texelCoord.x < size.x && texelCoord.y < size.y){
        float4 color = float4(0, 0, 0, 1);

        if(localThreadId.x != 0 && localThreadId.y != 0){
            color.x = float(texelCoord.x)/(size.x);
            color.y = float(texelCoord.y)/(size.y);
        }
        
        image[texelCoord] = color;
    }
}
