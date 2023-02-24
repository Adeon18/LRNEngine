cbuffer perFrame : register(b0)
{
    float4x4 worldToClip;
    float4x4 worldToClipInv;
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
    float4 modelToWorld0 : MODEL2WORLD0;
    float4 modelToWorld1 : MODEL2WORLD1;
    float4 modelToWorld2 : MODEL2WORLD2;
    float4 modelToWorld3 : MODEL2WORLD3;
    float4 color : COLOR;
};

struct VS_OUTPUT
{
	float4 outPos: SV_POSITION;
    float3 worldPos : POS;
    float4 outCol : COLOR;
    float3 worldNorm : NORM;
};

VS_OUTPUT main(VS_INPUT input)
{
	VS_OUTPUT output;
    float4x4 modelToWorld = float4x4(input.modelToWorld0, input.modelToWorld1, input.modelToWorld2, input.modelToWorld3);
    
    // Normal is transformed via transpose of the inverse
    float3 modelNorm = normalize(mul(float4(input.inNorm, 0.0f), transpose(meshToModelInv))).xyz;
    float4 modelPos = mul(float4(input.inPos, 1.0f), meshToModel);
    
    float4 worldPos = mul(modelPos, modelToWorld);
    float3 worldNorm = normalize(mul(float4(worldNorm, 0.0f), transpose(worldToClipInv))).xyz;
    
    output.outPos = mul(worldPos, worldToClip);
    output.worldPos = worldPos.xyz;
    output.outCol = input.color;
    output.worldNorm = modelNorm;
	return output;
}