cbuffer perFrame : register(b0)
{
    float4x4 worldToClip;
    float4x4 worldToClipInv;
    float4 iResolution;
    float4 iCameraPosition;
    float iTime;
};

struct VS_OUTPUT
{
    float4 clipPos : SV_POSITION;
    float3 modelPos : POS;
};

VS_OUTPUT main(inout float3 pos : POSITION)
{
    VS_OUTPUT output;
    float4 clipPos = mul(float4(pos, 0.0f), worldToClip);
    output.clipPos = float4(clipPos.x, clipPos.y, 0.0f, clipPos.w);
    output.modelPos = pos;
    return output;
}
