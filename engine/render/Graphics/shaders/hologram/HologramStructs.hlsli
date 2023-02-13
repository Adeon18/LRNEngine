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
    float4x4 modelToWorld : M2W;
    float3 modelPos : POSITION;
    float4 outCol : COLOR;
    float3 modelNorm : NORMAL;
};

struct PATCH_OUTPUT // 3 outer factors and 1 inner factor specifically for “tri” domain
{
    float EdgeFactors[3] : SV_TessFactor;
    float InsideFactor : SV_InsideTessFactor;
};

struct GS_OUTPUT
{
    float4 outPos : SV_POSITION;
    float3 modelPos : POS;
    float4 outCol : COLOR;
    float3 modelNorm : NORM;
};