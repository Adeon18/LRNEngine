#include "VisNormalStructs.hlsli"

#define ENABLE_VERTEX_NORMALS 0

static const float NORMAL_SIZE_SCALE = 0.2;
static const float4 FACE_NORMAL_COLOR = float4(0.f, 1.f, 0.f, 1.f);
static const float4 VERTEX_NORMAL_COLOR = float4(1.f, 1.f, 1.f, 1.f);


[maxvertexcount(8)]

// For now do only triangle normal
void main(triangle VS_OUTPUT input[3], inout LineStream<GS_OUTPUT> output)
{
    float3 triangleNorm = (input[0].worldNorm + input[1].worldNorm + input[2].worldNorm) / 3.0f;
    
    float3 triangleCenterWorld = (input[0].worldPos + input[1].worldPos + input[2].worldPos) / 3.0f;
    
    // Triangle Normal
    GS_OUTPUT normOrigin;
    normOrigin.outPos = mul(float4(triangleCenterWorld, 1.0f), worldToClip);
    normOrigin.outCol = FACE_NORMAL_COLOR;
    output.Append(normOrigin);
    
    GS_OUTPUT normTip;
    normTip.outPos = mul(float4(triangleCenterWorld + triangleNorm * NORMAL_SIZE_SCALE, 1.0f), worldToClip);
    normTip.outCol = FACE_NORMAL_COLOR;
    output.Append(normTip);

    output.RestartStrip();
    
#if ENABLE_VERTEX_NORMALS
    // Vertex Normals
    for (int i = 0; i < 3; ++i)
    {
        normOrigin.outPos = mul(float4(input[i].worldPos, 1.0f), worldToClip);
        normOrigin.outCol = VERTEX_NORMAL_COLOR;
        output.Append(normOrigin);
    
        normTip.outPos = mul(float4(input[i].worldPos + input[i].worldNorm * NORMAL_SIZE_SCALE, 1.0f), worldToClip);
        normTip.outCol = VERTEX_NORMAL_COLOR;
        output.Append(normTip);
        
        output.RestartStrip();
    }
#endif
}