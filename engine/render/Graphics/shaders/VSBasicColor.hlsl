cbuffer perFrame : register(b0)
{
    float4x4 worldToClip;
    float4 iResolution;
    float4 iCameraPosition;
    float iTime;
};

cbuffer perMesh : register(b1)
{
    float4x4 meshToModel;
    float4x4 meshToModelInv;
};

struct VS_INPUT
{
	float3 inPos : POSITION;
	float3 inNorm : NORMAL;
    float3 inTangent : TANGENT;
    float3 inBiTangent : BITANGENT;
    float3 inTC : TEXCOORD;
    float4 modelToWorld0 : W2CLIP0;
    float4 modelToWorld1 : W2CLIP1;
    float4 modelToWorld2 : W2CLIP2;
    float4 modelToWorld3 : W2CLIP3;
    float4 color : COLOR;
};

struct VS_OUTPUT
{
	float4 outPos: SV_POSITION;
    float3 modelPos : POS;
    float4 outCol : COLOR;
    float3 outNorm : NORM;
};

VS_OUTPUT main(VS_INPUT input)
{
	VS_OUTPUT output;
    float3 modelNorm = normalize(mul(float4(input.inNorm, 0.0f), meshToModelInv));
    float4 modelPos = mul(float4(input.inPos, 1.0f), meshToModel);
    
    float4x4 modelToWorld = float4x4(input.modelToWorld0, input.modelToWorld1, input.modelToWorld2, input.modelToWorld3);
    
    float4 worldPos = mul(modelPos, modelToWorld);
    
    output.outPos = mul(worldPos, worldToClip);
    output.modelPos = modelPos.xyz;
    output.outCol = input.color;
    output.outNorm = modelNorm;
	return output;
}