cbuffer perFrame : register(b0)
{
    float4x4 worldToClip;
};

struct VS_INPUT
{
	float3 inPos : POSITION;
    float4 modelToClip0 : M2CLIP0;
    float4 modelToClip1 : M2CLIP1;
    float4 modelToClip2 : M2CLIP2;
    float4 modelToClip3 : M2CLIP3;
    float4 color : COLOR;
};

struct VS_OUTPUT
{
	float4 outPos: SV_POSITION;
    float4 outCol: COLOR;
};

VS_OUTPUT main(VS_INPUT input)
{
	VS_OUTPUT output;
    float4x4 modelToClip = float4x4(input.modelToClip0, input.modelToClip1, input.modelToClip2, input.modelToClip3);
    output.outPos = mul(float4(input.inPos, 1.0f), modelToClip);
    output.outCol = input.color;
	return output;
}