// It's here if I need it.
cbuffer perFrame : register(b0)
{
    float4x4 worldToClip;
};

struct VS_INPUT
{
	float3 inPos : POSITION;
	float4 inColor : COLOR;
};

struct VS_OUTPUT
{
	float4 outPos: SV_POSITION;
	float4 outColor: COLOR;
};

VS_OUTPUT main(VS_INPUT input)
{
	VS_OUTPUT output;
    output.outPos = mul(float4(input.inPos, 1.0f), worldToClip);
	output.outColor = input.inColor;
	return output;
}