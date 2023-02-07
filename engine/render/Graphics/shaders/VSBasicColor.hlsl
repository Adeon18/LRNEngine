struct VS_INPUT
{
	float3 inPos : POSITION;
	float3 inNorm : NORMAL;
    float3 inTangent : TANGENT;
    float3 inBiTangent : BITANGENT;
    float3 inTC : TEXCOORD;
    float4 worldToClip0 : W2CLIP0;
    float4 worldToClip1 : W2CLIP1;
    float4 worldToClip2 : W2CLIP2;
    float4 worldToClip3 : W2CLIP3;
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
    float4x4 worldToClip = float4x4(input.worldToClip0, input.worldToClip1, input.worldToClip2, input.worldToClip3);
    output.outPos = mul(float4(input.inPos, 1.0f), worldToClip);
    output.outCol = input.color;
	return output;
}