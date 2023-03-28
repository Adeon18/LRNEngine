
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

float getSolidAngle(float3 fragPos, float3 lightPos, float radius)
{
    float distance = length(fragPos - lightPos);
    return 2 * PI * (1.0f - sqrt(max(1.0f - pow(radius / distance, 2), MIN_LIGHT_INTENCITY)));
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
    float NdotL = max(dot(norm, lightDir), MIN_LIGHT_INTENCITY);
    return (solidAngle * albedo * (1 - metalness) / PI) * (1 - fresnel(NdotL, F0));
}

float3 getCookTorrenceSpecular(float3 norm, float3 halfVector, float3 viewDir, float3 lightDir, float solidAngle, float roughness, float3 F0)
{
    float NdotV = max(dot(norm, viewDir), MIN_LIGHT_INTENCITY);
    float NdotL = max(dot(norm, lightDir), MIN_LIGHT_INTENCITY);
    float HdotL = max(dot(halfVector, lightDir), MIN_LIGHT_INTENCITY);
    float NdotH = max(dot(norm, halfVector), MIN_LIGHT_INTENCITY);
    return min(1, (ggx(roughness, NdotH) * solidAngle) / (4 * NdotV)) * smith(roughness, NdotV, NdotL) * fresnel(HdotL, F0);
}
