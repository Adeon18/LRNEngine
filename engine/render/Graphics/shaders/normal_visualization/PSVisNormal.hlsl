#include "VisNormalStructs.hlsli"


float4 main(GS_OUTPUT inp) : SV_TARGET
{
    return inp.outCol;
}