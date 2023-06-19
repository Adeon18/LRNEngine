struct VS_INPUT
{
    float3 inPos : POSITION;
    float3 inNorm : NORMAL;
    float3 inTangent : TANGENT;
    float3 inBiTangent : BITANGENT;
    float2 inTC : TEXCOORD;
    float4 decalToWorld0 : DECAL2WORLD0;
    float4 decalToWorld1 : DECAL2WORLD1;
    float4 decalToWorld2 : DECAL2WORLD2;
    float4 decalToWorld3 : DECAL2WORLD3;
    float4 decalToWorldInv0 : DECAL2WORLDINV0;
    float4 decalToWorldInv1 : DECAL2WORLDINV1;
    float4 decalToWorldInv2 : DECAL2WORLDINV2;
    float4 decalToWorldInv3 : DECAL2WORLDINV3;
};

struct PS_INPUT
{
    float4 outPos : SV_POSITION;
};

struct PS_OUTPUT_DEFERRED
{
    float4 albedo : SV_Target0;
    float4 normals : SV_Target1;
    float2 roughMet : SV_Target2;
    float4 emission : SV_Target3;
    uint objectIDs : SV_Target4;
};