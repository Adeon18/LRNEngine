struct PS_INPUT
{
	float4 outPos: SV_POSITION;
	float4 outColor: COLOR;
};

float4 main(PS_INPUT inp) : SV_TARGET
{
	return float4(0.5f, 0.7f, 0.2f, 1.0f);
}