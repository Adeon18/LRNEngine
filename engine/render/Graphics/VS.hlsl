float4 main(float2 inPos: POSITION) : SV_POSITION
{
	return float4(inPos, 0.0f, 1.0f);
}