cbuffer perFrame : register(b0)
{
    float4x4 worldToClip;
};

struct VS_INPUT
{
	float3 inPos : POSITION;
    float3 inPosInstance : INSTANCEPOS;
};

struct VS_OUTPUT
{
	float4 outPos: SV_POSITION;
};

VS_OUTPUT main(VS_INPUT input)
{
	VS_OUTPUT output;
    output.outPos = mul(float4(input.inPos, 1.0f), worldToClip);
    output.outPos += float4(input.inPosInstance, 1.0f);
	return output;
}