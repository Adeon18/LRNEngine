#ifndef LIGHTING_COOK_TORRANCE_HLSLI
#define LIGHTING_COOK_TORRANCE_HLSLI

#include "globals.hlsli"
#include "utility.hlsli"

static const float PI = 3.14159265f;
static const float MIN_LIGHT_INTENCITY = 0.0001f;

static const int MAX_POINTLIGHT_COUNT = 10;
static const int MAX_DIRLIGHT_COUNT = 1;

Texture2D g_textureSpotLight : TEXTURE : register(t16);


struct DirectionalLight
{
    float4 direction;
    
    float4 radiance;
    // float4 filled with solidAngle value
    float4 solidAngle;
};

struct PointLight
{
    float4 position;
    // X - constant, Y - linear, Z - quadratic

    float4 radiance;
    // float4 filled with radius value
    float4 radius;
};

struct SpotLight
{
    float4 position;
    float4 direction;
    
    float4x4 modelToWorld;
    float4x4 modelToWorldInv;
    
    float4 cutoffAngle;

    float4 radiance;
    // float4 filled with radius value
    float4 radius;
};


cbuffer perFrameLight : register(b2)
{
    // Has 4 ints that describe point light count
    int4 dirLightCount;
    int4 pointLightCount;
    DirectionalLight directLights[MAX_DIRLIGHT_COUNT];
    PointLight pointLights[MAX_POINTLIGHT_COUNT];
    SpotLight spotLight;
};

cbuffer perFrameLightFlags : register(b3)
{
    bool isDiffuseEnabled;
    bool isSpecularEnabled;
    bool isIBLEnabled;
};


// Determing which mip level to read in cubemap sampling with uniform/importance sampling
float hemisphereMip(float sampleProbability, float cubemapSize)
{
    float hemisphereTexels = cubemapSize * cubemapSize * 3;
    float log4 = 0.5 * log2(sampleProbability * hemisphereTexels);
    return log4;
}

// Input dir and NoD is N and NoL in a case of lighting computation 
void clampDirToHorizon(inout float3 dir, inout float NoD, float3 normal, float minNoD)
{
    if (NoD < minNoD)
    {
        dir = normalize(dir + (minNoD - NoD) * normal);
        NoD = minNoD;
    }
}

//void getKarisNdotH(float radiusTan, float NdotL, float NdotV, float VdotL)
//{
//    float radiusCos = rsqrt(1.0f * radiusTan * radiusTan);
//}

// [ de Carpentier 2017, "Decima Engine: Advances in Lighting and AA" ]
void SphereMaxNoH(float NoV, inout float NoL, inout float VoL, float SinAlpha, float CosAlpha, bool bNewtonIteration, out float NoH, out float VoH)
{
    float RoL = 2 * NoL * NoV - VoL;
    if (RoL >= CosAlpha)
    {
        NoH = 1;
        VoH = abs(NoV);
    }
    else
    {
        float rInvLengthT = SinAlpha * rsqrt(1 - RoL * RoL);
        float NoTr = rInvLengthT * (NoV - RoL * NoL);
        float VoTr = rInvLengthT * (2 * NoV * NoV - 1 - RoL * VoL);

        if (bNewtonIteration && SinAlpha != 0)
        {
			// dot( cross(N,L), V )
            float NxLoV = sqrt(saturate(1 - pow(NoL, 2) - pow(NoV, 2) - pow(VoL, 2) + 2 * NoL * NoV * VoL));

            float NoBr = rInvLengthT * NxLoV;
            float VoBr = rInvLengthT * NxLoV * 2 * NoV;

            float NoLVTr = NoL * CosAlpha + NoV + NoTr;
            float VoLVTr = VoL * CosAlpha + 1 + VoTr;

            float p = NoBr * VoLVTr;
            float q = NoLVTr * VoLVTr;
            float s = VoBr * NoLVTr;

            float xNum = q * (-0.5 * p + 0.25 * VoBr * NoLVTr);
            float xDenom = p * p + s * (s - 2 * p) + NoLVTr * ((NoL * CosAlpha + NoV) * pow(VoLVTr, 2) + q * (-0.5 * (VoLVTr + VoL * CosAlpha) - 0.5));
            float TwoX1 = 2 * xNum / (pow(xDenom, 2) + pow(xNum, 2));
            float SinTheta = TwoX1 * xDenom;
            float CosTheta = 1.0 - TwoX1 * xNum;
            NoTr = CosTheta * NoTr + SinTheta * NoBr;
            VoTr = CosTheta * VoTr + SinTheta * VoBr;
        }

        NoL = NoL * CosAlpha + NoTr;
        VoL = VoL * CosAlpha + VoTr;

        float InvLenH = rsqrt(2 + 2 * VoL);
        NoH = saturate((NoL + NoV) * InvLenH);
        VoH = saturate(InvLenH + InvLenH * VoL);
    }
}

float getSolidAngle(float3 fragPos, float3 lightPos, float radius)
{
    float distance = length(fragPos - lightPos);
    return 2 * PI * (1.0f - sqrt(max(1.0f - pow(radius / distance, 2), MIN_LIGHT_INTENCITY)));
}

// Get the falloff factor for a spherelight
float getHorizonAttenuation(float lightRadius, float3 lightPos, float3 fragPos)
{
    return min(1.0f, (length(lightPos - fragPos) + lightRadius) / (2 * lightRadius));
}

// Schlick's approximation of Fresnel reflectance,
float3 fresnel(float NdotL, float3 F0)
{
    return F0 + (1 - F0) * pow(1 - NdotL, 5);
}

// Height-correlated Smith G2 for GGX,
// Filament, 4.4.2 Geometric shadowing
float smith(float roughness, float NdotV, float NdotL)
{
    // HUH??? WHAT IS THIS???
    NdotV *= NdotV;
    NdotL *= NdotL;
    float roughnessSquared = roughness * roughness;
    return 2.0 / (sqrt(1 + roughnessSquared * (1 - NdotV) / NdotV) + sqrt(1 + roughnessSquared * (1 - NdotL) / NdotL));
}

// GGX normal distribution,
// Real-Time Rendering 4th Edition, page 340, equation 9.41
float ggx(float roughness, float NdotH)
{
    float roughnessSquared = roughness * roughness;

    float denom = NdotH * NdotH * (roughnessSquared - 1.0) + 1.0;
    denom = PI * denom * denom;
    return roughnessSquared / denom;
}

float3 getLambertDiffuse(float3 albedo, float3 norm, float3 lightDir, float3 F0, float metalness, float solidAngle)
{
    if (!isDiffuseEnabled)
    {
        return float3(0.0f, 0.0f, 0.0f);
    }
    float NdotL = max(dot(norm, lightDir), MIN_LIGHT_INTENCITY);
    return (solidAngle * albedo * (1 - metalness) / PI) * (1 - fresnel(NdotL, F0));
}

float3 getCookTorrenceSpecular(float3 micNorm, float3 halfVector, float3 viewDir, float3 lightDir, float solidAngle, float roughness, float3 F0)
{
    if (!isSpecularEnabled)
    {
        return float3(0.0f, 0.0f, 0.0f);

    }
    float NdotV = max(dot(micNorm, viewDir), MIN_LIGHT_INTENCITY);
    float NdotL = dot(micNorm, lightDir);
    float HdotL = max(dot(halfVector, lightDir), MIN_LIGHT_INTENCITY);
    float NdotH = max(dot(micNorm, halfVector), MIN_LIGHT_INTENCITY);
    float VdotL = dot(viewDir, lightDir);
    float VdotH;
    
    // Apply the carpenties method before specular compute
    // We can find spheresin and sphere cos with the help of the solidangle
    SphereMaxNoH(NdotV, NdotL, VdotL, sin(solidAngle / 2.0f), cos(solidAngle / 2.0f), false, NdotH, VdotH);
        
    return min(1, (ggx(roughness, NdotH) * solidAngle) / (4 * NdotV)) * smith(roughness, NdotV, NdotL) * fresnel(HdotL, F0);
}

float3 calculateDirectionalLight(DirectionalLight light, float3 micNorm, float3 viewDir, float3 albedo, float3 F0, float metallic, float roughness)
{
    float3 lightDir = -light.direction;
    float3 halfVector = normalize(viewDir + lightDir);
        
    float3 NdotL = max(dot(micNorm, lightDir), MIN_LIGHT_INTENCITY);
        
    float solidAngle = light.solidAngle;
        
    return light.radiance.xyz *
        (getLambertDiffuse(albedo, micNorm, lightDir, F0, metallic, solidAngle) * NdotL +
            getCookTorrenceSpecular(micNorm, halfVector, viewDir, lightDir, solidAngle, roughness, F0));
}

float3 calculatePointLight(PointLight light, float3 micNorm, float3 macNorm, float3 fragWorldPos, float3 viewDir, float3 albedo, float3 F0, float metallic, float roughness)
{
    //float3 lightDir = normalize(light.position.xyz - fragWorldPos);
    
    // This code is taken from here https://alextardif.com/arealights.html
    // Instead of having the light direction as to the center of a sphere, we take it to
    // the closest point ABOVE HORIZON
    float3 r = reflect(-viewDir, micNorm);
    float3 lightDir = light.position.xyz - fragWorldPos;
    float3 centerToRay = (dot(lightDir, r) * r) - lightDir;
    float3 closestPoint = lightDir + centerToRay * saturate(light.radius.x / length(centerToRay));
    lightDir = normalize(closestPoint);

    float3 halfVector = normalize(viewDir + lightDir);
    
    // Discard if below horizon
    float NdotL = dot(macNorm, lightDir);
    if (NdotL < MIN_LIGHT_INTENCITY)
    {
        return float3(0, 0, 0);
    }
        
    float solidAngle = getSolidAngle(fragWorldPos, light.position.xyz, light.radius.x);
    
    // Get the falloff factor when light hides at horizon
    float falloff = getHorizonAttenuation(light.radius.x, light.position.xyz, fragWorldPos);
        
    return light.radiance.xyz * falloff *
        (getLambertDiffuse(albedo, micNorm, lightDir, F0, metallic, solidAngle) * NdotL +
            getCookTorrenceSpecular(micNorm, halfVector, viewDir, lightDir, solidAngle, roughness, F0));
}

float3 calculateSpotLight(SpotLight light, float3 micNorm, float3 fragWorldPos, float3 viewDir, float3 albedo, float3 F0, float metallic, float roughness)
{
    //! Get the spotlight texture color
    const float COS_CUTOFF_ANGLE = cos(light.cutoffAngle.x);
    const float TAN_CUTOFF_ANGLE = tan(light.cutoffAngle.x);
    
    float3 fragPosModelLight = mul(float4(fragWorldPos, 1.0f), light.modelToWorldInv);

    float spotLightRadius = fragPosModelLight.z * TAN_CUTOFF_ANGLE;
    
    float u = remap(-spotLightRadius, spotLightRadius, 0, 1, fragPosModelLight.x);
    float v = remap(-spotLightRadius, spotLightRadius, 0, 1, fragPosModelLight.y);

    float3 flashMask = g_textureSpotLight.Sample(g_linearWrap, float2(u, v));
    
    //! Lighting calculations
    float3 lightDir = normalize(light.position.xyz - fragWorldPos);
    float theta = dot(lightDir, -light.direction.xyz);
    
    // We smooth the edges of spotlight
    float epsilon = 0.05f;
    float intensity = clamp((theta - COS_CUTOFF_ANGLE) / epsilon, 0.0, 1.0);

    float3 halfVector = normalize(viewDir + lightDir);
        
    float3 NdotL = max(dot(micNorm, lightDir), MIN_LIGHT_INTENCITY);
        
    float solidAngle = getSolidAngle(fragWorldPos, light.position.xyz, light.radius.x);
        
    return flashMask * light.radiance.xyz * intensity *
        (getLambertDiffuse(albedo, micNorm, lightDir, F0, metallic, solidAngle) * NdotL +
            getCookTorrenceSpecular(micNorm, halfVector, viewDir, lightDir, solidAngle, roughness, F0));
}

#endif // LIGHTING_COOK_TORRANCE_HLSLI