#include "../globals.hlsli"

struct VS_OUTPUT
{
    float4 clipPos : SV_POSITION;
    float4 color : COLOR;
};


float4 main(VS_OUTPUT inp) : SV_TARGET
{
    return inp.color;
}