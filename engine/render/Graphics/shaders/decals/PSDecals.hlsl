#include "../globals.hlsli"
#include "../utility.hlsli"

#include "DecalStructs.hlsli"


cbuffer perFrame : register(b0)
{
    float4x4 worldToClip;
    float4x4 worldToClipInv;
    float4 iResolution;
    float4 iCameraPosition;
    float4 iPFSCubemapResolution;
    float iTime;
};



Texture2D g_splatterNorm : TEXTURE : register(t1);
Texture2D<uint> g_objectIds : TEXTURE : register(t3);
Texture2D g_normalsBuffer : TEXTURE : register(t4);
Texture2D g_depthBuffer : TEXTURE : register(t5);

float3 PSPositionFromDepth(float2 vTexCoord)
{
    // Get the depth value for this pixel
    float z = g_depthBuffer.Sample(g_pointWrap, float2(vTexCoord.x, vTexCoord.y)).r;
    // Get x/w and y/w from the viewport position
    float x = vTexCoord.x * 2 - 1;
    float y = (1 - vTexCoord.y) * 2 - 1;
    float4 vProjectedPos = float4(x, y, z, 1.0f);
    // Transform by the inverse projection matrix
    float4 vPositionVS = mul(vProjectedPos, worldToClipInv);
    // Divide by w to get the view-space position
    return vPositionVS.xyz / vPositionVS.w;
}

float3 getNormalFromTexture(float2 texCoords, float3x3 TBN)
{
    float3 normFromTex = g_splatterNorm.Sample(g_pointWrap, texCoords).xyz;
    normFromTex = normFromTex * 2.0f - 1.0f;
    normFromTex = normalize(mul(normFromTex, TBN));
    
    return normFromTex;
}

PS_OUTPUT_DEFERRED main(PS_INPUT inp) : SV_TARGET
{
    PS_OUTPUT_DEFERRED output;
    
    float2 sampleCoords = inp.outPos.xy / iResolution.xy;
    
    float3 worldPos = PSPositionFromDepth(sampleCoords);
    
    float zFromBuf = g_depthBuffer.Sample(g_pointWrap, sampleCoords).r;
    
    float3 decalPos = mul(float4(worldPos, 1.0f), inp.worldToDecal);
    
    if (decalPos.x > 1.0f || decalPos.x < -1.0f ||
        decalPos.y > 1.0f || decalPos.y < -1.0f ||
        decalPos.z > 1.0f || decalPos.z < -1.0f)
    {
        discard;
    }
    
    float2 normSampleCoords;
    normSampleCoords.x = decalPos.x * 0.5 + 0.5;
    normSampleCoords.y = decalPos.y * -0.5 + 0.5;
    
    uint id = g_objectIds.Load(int3(inp.outPos.xy, 0));
    if (id != inp.parentObjectID)
        discard;
    
    // Discard bakc faces
    float3 currentNorm = unpackOctahedron(g_normalsBuffer.Sample(g_pointWrap, sampleCoords).rg);
    if (dot(currentNorm, inp.decalToWorld[2].xyz) < 0.0001f)
    {
        discard;
    }
    
    // TODO: Excess operations
    float4 norm = g_splatterNorm.Sample(g_pointWrap, normSampleCoords);
    if (norm.a == 0)
        discard;
    
    float3 N = currentNorm;
    float3 T = inp.decalToWorld[0];
    T = normalize(T - N * dot(N, T));
    float3 B = normalize(cross(N, T));
    float3x3 TBN = float3x3(T, B, N);
    float3 decalMicNorm = getNormalFromTexture(normSampleCoords, TBN);
    
    float3 col = float3(1.0f, 0.0f, 1.0f);
    output.albedo = float4(col, 1.0f);
    output.normals = float4(packOctahedron(currentNorm), packOctahedron(decalMicNorm));
    output.roughMet = float2(0.1, 0.1);
    output.emission = float4(0.0f, 0.0f, 0.0f, 0.0f);
    //output.objectIDs = 0;
    
    return output;
}