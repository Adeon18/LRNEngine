cbuffer perFrame : register(b0)
{
    float4 iResolution;
    float4 iCameraPosition;
    float iTime;
};


struct PS_INPUT
{
    float4 outPos : SV_POSITION;
    float4 outCol : COLOR;
};


float4 main(PS_INPUT inp) : SV_TARGET
{
    return inp.outCol;
}