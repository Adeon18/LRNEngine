#include "../sample_util.hlsli"

struct VS_OUTPUT
{
    float4 clipPos : SV_POSITION;
};

float4 main(VS_OUTPUT inp) : SV_TARGET
{
    return float4(1.0f, 1.0f, 1.0f, 1.0f);
}