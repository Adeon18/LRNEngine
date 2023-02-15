#include "VisWireframeStructs.hlsli"


static const float4 WIREFRAME_COLOR = float4(1.f, 1.f, 1.f, 1.f);


[maxvertexcount(6)]
// For now do only triangle normal
void main(triangle VS_OUTPUT input[3], inout LineStream<GS_OUTPUT> output)
{   
    // Draw 3 lines that construct a triangle
    for (int i = 0; i < 3; ++i)
    {
        GS_OUTPUT normOrigin;
        normOrigin.outPos = mul(float4(input[i].worldPos, 1.0f), worldToClip);
        normOrigin.outCol = WIREFRAME_COLOR;
        output.Append(normOrigin);
    
        GS_OUTPUT normTip;
        normTip.outPos = mul(float4(input[(i + 1) % 3].worldPos, 1.0f), worldToClip);
        normTip.outCol = WIREFRAME_COLOR;
        output.Append(normTip);
        
        output.RestartStrip();
    }
}