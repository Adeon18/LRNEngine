float4 main(float2 inPos: POSITION) : SV_POSITION // SV_POSITION specifies that we return position
{
	return float4(inPos, 0, 1);
}