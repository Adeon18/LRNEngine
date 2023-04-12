struct VS_IN
{
    uint vertexId : SV_VertexID;
};

struct VS_OUTPUT
{
    float4 clipPos : SV_POSITION;
};

VS_OUTPUT main(VS_IN input)
{
    VS_OUTPUT output;
    // Fill each vertex separately depending on vertex ID => CLOCKVISE
    if (input.vertexId == 0)
    {
        output.clipPos = float4(-1.0f, -1.0f, 0.0f, 1.0f);
    }
    else if (input.vertexId == 2)
    {
        output.clipPos = float4(3.0f, -1.0f, 0.0f, 1.0f);
    }
    else if (input.vertexId == 1)
    {
        output.clipPos = float4(-1.0f, 3.0f, 0.0f, 1.0f);
    }
    return output;
}