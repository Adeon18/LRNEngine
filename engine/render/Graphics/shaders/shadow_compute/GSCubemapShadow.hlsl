cbuffer perLight : register(b0)
{
    float4x4 projectionMatrix;
    float4x4 viewMatrices[6];
};


struct GS_INPUT
{
    float4 outWorldPos : POSITION;
};

struct GS_OUTPUT
{
    float4 outPos : SV_POSITION;
    uint RTIndex : SV_RenderTargetArrayIndex;
};


[maxvertexcount(18)]
void main(triangle GS_INPUT input[3], inout TriangleStream<GS_OUTPUT> CubeMapStream)
{
    for (int f = 0; f < 6; ++f)
    {
        GS_OUTPUT output;
        output.RTIndex = f;

        for (int v = 0; v < 3; ++v)
        {
            float4 worldPosition = input[v].outWorldPos;
            float4 viewPosition = mul(worldPosition, viewMatrices[f]);
            output.outPos = mul(viewPosition, projectionMatrix);

            CubeMapStream.Append(output);
        }

        CubeMapStream.RestartStrip();
    }
}