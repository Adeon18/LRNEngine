cbuffer perFrame : register(b0)
{
    float4x4 worldToClip;
};

struct VS_INPUT
{
    float3 inPos : POSITION;
    float3 inNorm : NORMAL;
    float3 inTangent : TANGENT;
    float3 inBiTangent : BITANGENT;
    float2 inTC : TEXCOORD;
    float4 modelToWorld0 : MODEL2WORLD0;
    float4 modelToWorld1 : MODEL2WORLD1;
    float4 modelToWorld2 : MODEL2WORLD2;
    float4 modelToWorld3 : MODEL2WORLD3;
    float4 modelToWorldInv0 : MODEL2WORLDINV0;
    float4 modelToWorldInv1 : MODEL2WORLDINV1;
    float4 modelToWorldInv2 : MODEL2WORLDINV2;
    float4 modelToWorldInv3 : MODEL2WORLDINV3;
    float4 color : COLOR;
};


struct VS_OUTPUT
{
    float4 clipPos : SV_POSITION;
};

VS_OUTPUT main(VS_INPUT input)
{
    VS_OUTPUT output;
    
    output.clipPos = float4(0, 0, 0, 0);
    
    return output;
}
