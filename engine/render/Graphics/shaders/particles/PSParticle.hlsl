#include "../globals.hlsli"
#include "../lighting_cook_torrance.hlsli"

struct VS_OUTPUT
{
    float4 clipPos : SV_POSITION;
    float4 worldPos : WORLD_POS;
    float3 norm : NORM;
    float4 color : COLOR;
    float2 uv : TEXCOORD;
    float spawnTime : SPAWNTIME;
};

cbuffer perFrame : register(b0)
{
    float4 iResolution;
    float4 iCameraPosition;
    float4 iPFSCubemapResolution;
    float iTime;
};

cbuffer perFrame2 : register(b1)
{
    int4 atlasFrameCount;
    int4 animationSpeedFPS;
};

Texture2D g_textureDBF : TEXTURE : register(t0);
Texture2D g_textureMVEA : TEXTURE : register(t1);
Texture2D g_textureRLU : TEXTURE : register(t2);

float computeLightMapFactor(float3 lightDir, float3 lightMapRLU, float3 lightMapDBF)
{
    float hMap = (lightDir.x > 0.0f) ? (lightMapRLU.r) : (lightMapRLU.g); // Picks the correct horizontal side.
    float vMap = (lightDir.y > 0.0f) ? (lightMapRLU.b) : (lightMapDBF.r); // Picks the correct Vertical side.
    float dMap = (lightDir.z > 0.0f) ? (lightMapDBF.b) : (lightMapDBF.g); // Picks the correct Front/back Pseudo Map
    float lightMap = hMap * lightDir.x * lightDir.x + vMap * lightDir.y * lightDir.y + dMap * lightDir.z * lightDir.z; // Pythagoras!
    return lightMap;
}

//float getLightFactorFromBasis(float3 vec)

static const float3 DIRECTIONS[6] =
{
    float3(1.0f, 0.0f, 0.0f),
    float3(-1.0f, 0.0f, 0.0f),
    float3(0.0f, 1.0f, 0.0f),
    float3(0.0f, -1.0f, 0.0f),
    float3(0.0f, 0.0f, 1.0f),
    float3(0.0f, 0.0f, -1.0f),
};


float4 main(VS_OUTPUT inp) : SV_TARGET
{
    float atlasTexWidth = 1.0f / atlasFrameCount.x;
    float atlasTexHeight = 1.0f / atlasFrameCount.z;
        
    float lived = iTime - inp.spawnTime;
    
    int textureIdx = lived * animationSpeedFPS.x;
    
    float2 uvOffset = float2(int(textureIdx % atlasFrameCount.x) * atlasTexWidth, int(textureIdx / atlasFrameCount.x) * atlasTexHeight);
    
    float2 finalUV = uvOffset + float2(inp.uv.x * atlasTexWidth, inp.uv.y * atlasTexWidth);
    
    float4 texCol = g_textureMVEA.Sample(g_linearWrap, finalUV);
    float3 lightMapRLU = g_textureRLU.Sample(g_linearWrap, finalUV);
    float3 lightMapDBF = g_textureDBF.Sample(g_linearWrap, finalUV);
   
    float3 outRad = texCol.z * inp.color.rgb * 0.05f;
    
    for (int i = 0; i < dirLightCount.x; ++i)
    {
        float3 toLight = -directLights[i].direction;
        for (int j = 0; j < 6; ++j)
        {
            outRad += directLights[i].radiance * directLights[i].solidAngle * computeLightMapFactor(DIRECTIONS[i], lightMapRLU, lightMapDBF) * max(0.0f, dot(toLight, DIRECTIONS[j]));
        }
    }

    for (int i = 0; i < pointLightCount.x; ++i)
    {
        float3 toLight = normalize(pointLights[i].position.xyz - inp.worldPos.xyz);
        float solidAngle = getSolidAngle(inp.worldPos.xyz, pointLights[i].position.xyz, pointLights[i].radius.x);
        for (int j = 0; j < 6; ++j)
        {
            outRad += pointLights[i].radiance * solidAngle * computeLightMapFactor(DIRECTIONS[i], lightMapRLU, lightMapDBF) * max(0.0f, dot(toLight, DIRECTIONS[j]));
        }
    }
    
    //! Spotlight
    {
        float3 toLight = normalize(spotLight.position.xyz - inp.worldPos.xyz);
        float solidAngle = getSolidAngle(inp.worldPos.xyz, spotLight.position.xyz, spotLight.radius.x);
        for (int j = 0; j < 6; ++j)
        {
            outRad += pointLights[i].radiance * solidAngle * computeLightMapFactor(DIRECTIONS[i], lightMapRLU, lightMapDBF) * max(0.0f, dot(toLight, DIRECTIONS[j]));
        }
    }
    
    return float4(inp.color.rgb * outRad.rgb, inp.color.a * texCol.a);
}