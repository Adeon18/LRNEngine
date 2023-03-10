struct VS_IN
{
    uint vertexId : SV_VertexID;
};

struct VS_OUTPUT
{
    float4 clipPos : SV_POSITION;
    float2 texCoords : TEXCOORD;
};

VS_OUTPUT main(VS_IN input)
{
    VS_OUTPUT output;
    // Fill each vertex separately depending on vertex ID => CLOCKVISE
    // Bottom left
    if (input.vertexId == 0)
    {
        output.clipPos = float4(-1.0f, -1.0f, 0.0f, 1.0f);
        output.texCoords = float2(0, 1);
    }
    // Bottom right
    else if (input.vertexId == 2)
    {
        output.clipPos = float4(3.0f, -1.0f, 0.0f, 1.0f);
        output.texCoords = float2(2, 1);
    }
    // Top left
    else if (input.vertexId == 1)
    {
        output.clipPos = float4(-1.0f, 3.0f, 0.0f, 1.0f);
        output.texCoords = float2(0, -1);

    }
    return output;
}