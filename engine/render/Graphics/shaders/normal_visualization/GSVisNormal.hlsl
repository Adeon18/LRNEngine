#include "VisNormalStructs.hlsli"

static const float NORMAL_SIZE_SCALE = 0.1;


[maxvertexcount(8)]

// For now do only triangle normal
void main(triangle VS_OUTPUT input[3], inout LineStream<GS_OUTPUT> output)
{
    float3 triangleNorm = (input[0].worldNorm + input[1].worldNorm + input[2].worldNorm) / 3.0f;
    
    float3 triangleCenterWorld = (input[0].worldPos + input[1].worldPos + input[2].worldPos) / 3.0f;
    
    
    GS_OUTPUT normOrigin;
    normOrigin.outPos = mul(float4(triangleCenterWorld, 1.0f), worldToClip);
    normOrigin.outCol = float4(1.0f, 1.0f, 1.0f, 1.0f);
    output.Append(normOrigin);
    
    GS_OUTPUT normTip;
    normTip.outPos = mul(float4(triangleCenterWorld + triangleNorm * NORMAL_SIZE_SCALE, 1.0f), worldToClip);
    normTip.outCol = float4(1.0f, 1.0f, 1.0f, 1.0f);
    output.Append(normTip);
}