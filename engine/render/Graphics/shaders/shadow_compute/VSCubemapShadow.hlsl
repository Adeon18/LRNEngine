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

struct PS_INPUT
{
    float4 outWorldPos : POSITION;
};


PS_INPUT main(VS_INPUT input)
{
    PS_INPUT output;
    float4x4 modelToWorld = float4x4(input.modelToWorld0, input.modelToWorld1, input.modelToWorld2, input.modelToWorld3);
    
    float4 modelPos = mul(float4(input.inPos, 1.0f), meshToModel);
    
    float4 worldPos = mul(modelPos, modelToWorld);

    output.outWorldPos = worldPos;

    return output;
}