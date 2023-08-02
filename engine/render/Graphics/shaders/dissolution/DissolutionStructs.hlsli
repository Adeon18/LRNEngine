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
    float4 time : TIME;
};

struct PS_INPUT
{
    float4 outPos : SV_POSITION;
    float3 worldPos : POS;
    float3 worldNorm : NORM;
    float2 outTexCoord : TEXCOORD;
    float4 outTime : TIME;
    float3x3 TBN : TBN;
};

struct PS_OUTPUT_DEFERRED
{
    float4 albedo : SV_Target0;
    float4 normals : SV_Target1;
    float2 roughMet : SV_Target2;
    float4 emission : SV_Target3;
    uint objectIDs : SV_Target4;
};