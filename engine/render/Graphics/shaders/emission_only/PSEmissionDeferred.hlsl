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


struct PS_OUTPUT_DEFERRED
{
    float4 albedo : SV_Target0;
    float4 normals : SV_Target1;
    float2 roughMet : SV_Target2;
    float4 emission : SV_Target3;
    uint objectIDs : SV_Target4;
};


PS_OUTPUT_DEFERRED main(PS_INPUT inp) : SV_TARGET
{
    PS_OUTPUT_DEFERRED output;
    output.albedo = float4(0, 0, 0, 0);
    output.normals = float4(0, 0, 0, 0);
    output.roughMet = float4(0, 0, 0, 0);
    output.emission = inp.outCol;
    output.objectIDs = 0;
    
    return output;
}