#include "../globals.hlsli"
#include "../lighting_cook_torrance.hlsli"

struct VS_OUTPUT
{
    float4 clipPos : SV_POSITION;
    float4 worldPos : WORLD_POS;
    float3 norm : NORM;
    float4 color : COLOR;
    float2 uv : TEXCOORD;
    float2 screenUV : TEXCOORD2;
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
Texture2D g_depthTexture : TEXTURE : register(t3);

float computeLightMapFactor(float3 lightDir, float3 lightMapRLU, float3 lightMapDBF)
{
    float hMap = (lightDir.x > 0.0f) ? (lightMapRLU.r) : (lightMapRLU.g); // Picks the correct horizontal side.
    float vMap = (lightDir.y > 0.0f) ? (lightMapRLU.b) : (lightMapDBF.r); // Picks the correct Vertical side.
    float dMap = (lightDir.z > 0.0f) ? (lightMapDBF.b) : (lightMapDBF.g); // Picks the correct Front/back Pseudo Map
    float lightMap = hMap * lightDir.x * lightDir.x + vMap * lightDir.y * lightDir.y + dMap * lightDir.z * lightDir.z; // Pythagoras!
    return lightMap;
}

static const float3 DIRECTIONS[6] =
{
    float3(1.0f, 0.0f, 0.0f),
    float3(-1.0f, 0.0f, 0.0f),
    float3(0.0f, 1.0f, 0.0f),
    float3(0.0f, -1.0f, 0.0f),
    float3(0.0f, 0.0f, 1.0f),
    float3(0.0f, 0.0f, -1.0f),
};

static const float THICKNESS = 40.0f;


float4 main(VS_OUTPUT inp) : SV_TARGET
{
    float atlasTexWidth = 1.0f / atlasFrameCount.x;
    float atlasTexHeight = 1.0f / atlasFrameCount.z;
    
    float oneFrameTime = 1.0f / animationSpeedFPS.x;
    float totalLifetime = atlasFrameCount.x * atlasFrameCount.z / animationSpeedFPS.x;
    
    float lived = min(iTime - inp.spawnTime, 4.0f);
    
    int textureIdx = floor(lived * animationSpeedFPS.x);
    float actualTime = textureIdx * oneFrameTime;
    
    float timeFraction = (lived - actualTime) / oneFrameTime;
    
    float2 uvOffsetThis = float2(int(textureIdx % atlasFrameCount.x) * atlasTexWidth, floor(textureIdx / atlasFrameCount.x) * atlasTexHeight);
    ++textureIdx;
    float2 uvOffsetNext = float2(int(textureIdx % atlasFrameCount.x) * atlasTexWidth, floor(textureIdx / atlasFrameCount.x) * atlasTexHeight);
    
    float2 finalUVThis = clamp(uvOffsetThis + float2(inp.uv.x * atlasTexWidth, inp.uv.y * atlasTexWidth), 0.0f, 1.0f);
    float2 finalUVNext = clamp(uvOffsetNext + float2(inp.uv.x * atlasTexWidth, inp.uv.y * atlasTexWidth), 0.0f, 1.0f);
    
    // ----------- sample motion-vectors -----------

    float2 mv0 = 2.0 * g_textureMVEA.Sample(g_linearWrap, finalUVNext).rg - 1.0; // current frame motion-vector
    float2 mv1 = 2.0 * g_textureMVEA.Sample(g_linearWrap, finalUVNext).rg - 1.0; // next frame motion-vector

    // need to flip motion-vector Y specifically for the smoke textures:
    mv0.y = -mv0.y;
    mv1.y = -mv1.y;

    // ----------- UV flowing along motion-vectors -----------

    static const float MV_SCALE = 0.0015; // adjusted for the smoke textures
    float time = timeFraction; // goes from 0.0 to 1.0 between two sequential frames

    float2 uv0 = finalUVThis; // texture sample uv for the current frame
    uv0 -= mv0 * MV_SCALE * time; // if MV points in some direction, then UV flows in opposite

    float2 uv1 = finalUVNext; // texture sample uv for the next frame
    uv1 -= mv1 * MV_SCALE * (time - 1.f); // if MV points in some direction, then UV flows in opposite

    // ----------- sample textures -----------

    float2 emissionAlpha0 = g_textureMVEA.Sample(g_linearWrap, uv0).ba;
    float2 emissionAlpha1 = g_textureMVEA.Sample(g_linearWrap, uv1).ba;

    // .x - right, .y - left, .z - up
    float3 lightmapRLU0 = g_textureRLU.Sample(g_linearWrap, uv0).rgb;
    float3 lightmapRLU1 = g_textureRLU.Sample(g_linearWrap, uv1).rgb;

    // .x - down, .y - back, .z - front
    float3 lightmapDBF0 = g_textureDBF.Sample(g_linearWrap, uv0).rgb;
    float3 lightmapDBF1 = g_textureDBF.Sample(g_linearWrap, uv1).rgb;

    // ----------- lerp values -----------

    float2 emissionAlpha = lerp(emissionAlpha0, emissionAlpha1, time);
    float3 lightMapRLU = lerp(lightmapRLU0, lightmapRLU1, time);
    float3 lightMapDBF = lerp(lightmapDBF0, lightmapDBF1, time);

    
    //float4 texCol = g_textureMVEA.Sample(g_linearWrap, finalUV);
    //float3 lightMapRLU = g_textureRLU.Sample(g_linearWrap, finalUV);
    //float3 lightMapDBF = g_textureDBF.Sample(g_linearWrap, finalUV);
   
    float3 outRad = emissionAlpha.x * inp.color.rgb * 0.05f;
    
    for (int i = 0; i < dirLightCount.x; ++i)
    {
        float3 toLight = -directLights[i].direction;
        for (int j = 0; j < 6; ++j)
        {
            outRad += directLights[i].radiance * directLights[i].solidAngle * computeLightMapFactor(DIRECTIONS[j], lightMapRLU, lightMapDBF) * max(0.0f, dot(toLight, DIRECTIONS[j]));
        }
    }

    for (int i = 0; i < pointLightCount.x; ++i)
    {
        float3 toLight = normalize(pointLights[i].position.xyz - inp.worldPos.xyz);
        float solidAngle = getSolidAngle(inp.worldPos.xyz, pointLights[i].position.xyz, pointLights[i].radius.x);
        for (int j = 0; j < 6; ++j)
        {
            outRad += pointLights[i].radiance * solidAngle * computeLightMapFactor(DIRECTIONS[j], lightMapRLU, lightMapDBF) * max(0.0f, dot(toLight, DIRECTIONS[j]));
        }
    }
    
    //! Spotlight
    {
        float3 toLight = normalize(spotLight.position.xyz - inp.worldPos.xyz);
        float solidAngle = getSolidAngle(inp.worldPos.xyz, spotLight.position.xyz, spotLight.radius.x);
        for (int j = 0; j < 6; ++j)
        {
            outRad += pointLights[i].radiance * solidAngle * computeLightMapFactor(DIRECTIONS[j], lightMapRLU, lightMapDBF) * max(0.0f, dot(toLight, DIRECTIONS[j]));
        }
    }
    
    
    float closestDepth = g_depthTexture.Sample(g_pointWrap, inp.screenUV).r;
    float particleDepth = inp.clipPos.z;
    
    float depthDiff = 1.0f / closestDepth - 1.0f / particleDepth;
   
    float finalAlpha = inp.color.a * emissionAlpha.y;
    
    //if (depthDiff < THICKNESS && (finalAlpha > 0.01f))
    //{
        //finalAlpha = max(finalAlpha - min(((THICKNESS - depthDiff) / THICKNESS), finalAlpha - 0.1f), 0.0f);
    //}
    
    return float4(inp.color.rgb * outRad.rgb, finalAlpha);
}