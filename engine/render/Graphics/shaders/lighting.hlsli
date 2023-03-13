#include "globals.hlsli"

#define MAX_POINTLIGHT_COUNT 10
#define MAX_DIRLIGHT_COUNT 1

float remap(float low1, float high1, float low2, float high2, float value)
{
    return low2 + (value - low1) * (high2 - low2) / (high1 - low1);
}

Texture2D g_textureSpotLight : TEXTURE : register(t16);

//! For now, material placeholders
static const float3 g_materialAmbient = float3(0.05f, 0.05f, 0.05f);
static const float3 g_materialDiffuse = float3(0.8f, 0.8f, 0.8f);
static const float3 g_materialSpecular = float3(1.0f, 1.0f, 1.0f);

struct DirectionalLight
{
    float4 direction;
    
    float4 intensity;
    float4 color;
};

struct PointLight
{
    float4 position;
    // X - constant, Y - linear, Z - quadratic
    float4 distProperties;
    
    float4 intensity;
    float4 color;
};

struct SpotLight
{
    float4 position;
    float4 direction;
    
    float4x4 modelToWorld;
    float4x4 modelToWorldInv;
    
    float4 cutoffAngle;
    
    // X - constant, Y - linear, Z - quadratic
    float4 distProperties;
    
    float4 intensity;
    float4 color;
};


cbuffer perFrameLight : register(b1)
{
    // Has 4 ints that describe point light count
    int4 dirLightCount;
    int4 pointLightCount;
    DirectionalLight directLights[MAX_DIRLIGHT_COUNT];
    PointLight pointLights[MAX_POINTLIGHT_COUNT];
    SpotLight spotLight;
};

#define BLINN 1

float3 calculateDirectionalLight(DirectionalLight light, float3 norm, float3 toCam, float3 inFragTexCol)
{
    float3 lightDir = normalize(-light.direction.xyz);
    // diffuse shading
    float diff = max(dot(norm, lightDir), 0.0001);
    // specular shading
    #if BLINN == 1
        float3 halfwayDir = normalize(lightDir + toCam);
        float spec = pow(max(dot(norm, halfwayDir), 0.0), 32.0f);
    #else
        float3 reflectDir = reflect(-lightDir, norm);
        float spec = pow(max(dot(toCam, reflectDir), 0.0001), 16.0f);
    #endif
    
    // combine results
    float3 ambient = inFragTexCol * g_materialAmbient;
    float3 diffuse = diff * inFragTexCol * g_materialDiffuse;
    float3 specular = spec * inFragTexCol * g_materialSpecular;
    // Apply color
    return light.intensity.xyz * light.color.xyz * (ambient + diffuse + specular);
}

// calculates the color when using a point light.
float3 calculatePointLight(PointLight light, float3 norm, float3 fragWorldPos, float3 toCam, float3 inFragTexCol)
{
    float constant = light.distProperties.x;
    float lin = light.distProperties.y;
    float quadratic = light.distProperties.z;
    
    float3 lightDir = normalize(light.position.xyz - fragWorldPos);
    // diffuse shading
    float diff = max(dot(norm, lightDir), 0.0001);
    // specular shading
    #if BLINN == 1
        float3 halfwayDir = normalize(lightDir + toCam);
        float spec = pow(max(dot(norm, halfwayDir), 0.0001), 32.0f);
    #else
        float3 reflectDir = reflect(-lightDir, norm);
        float spec = pow(max(dot(toCam, reflectDir), 0.0001), 16.0f);
    #endif
    // attenuation
    float distance = length(light.position.xyz - fragWorldPos);
    float attenuation = 1.0 / (constant + lin * distance + quadratic * (distance * distance)) * light.intensity.xyz;
    // combine results
    float3 ambient = inFragTexCol * g_materialAmbient;
    float3 diffuse = diff * inFragTexCol * g_materialDiffuse;
    float3 specular = spec * inFragTexCol * g_materialSpecular;

    ambient *= attenuation;
    diffuse *= attenuation;
    specular *= attenuation;
    // Apply color
    return light.color.xyz * (ambient + diffuse + specular);
}

float3 calculateSpotLight(SpotLight light, float3 norm, float3 fragWorldPos, float3 toCam, float3 inFragTexCol)
{
    const float COS_CUTOFF_ANGLE = cos(light.cutoffAngle.x);
    const float TAN_CUTOFF_ANGLE = tan(light.cutoffAngle.x);
    
    float3 fragPosModelLight = mul(float4(fragWorldPos, 1.0f), light.modelToWorldInv);

    float spotLightRadius = fragPosModelLight.z * TAN_CUTOFF_ANGLE;
    
    float u = remap(-spotLightRadius, spotLightRadius, 0, 1, fragPosModelLight.x);
    float v = remap(-spotLightRadius, spotLightRadius, 0, 1, fragPosModelLight.y);

    float3 flashMask = g_textureSpotLight.Sample(g_linearWrap, float2(u, v));
    
    float constant = light.distProperties.x;
    float lin = light.distProperties.y;
    float quadratic = light.distProperties.z;
    
    float3 lightDir = normalize(light.position.xyz - fragWorldPos);
    float theta = dot(lightDir, -light.direction.xyz);
    
    if (theta < COS_CUTOFF_ANGLE)
    {
        return g_materialAmbient * inFragTexCol;
    }
        
    // diffuse shading
    float diff = max(dot(norm, lightDir), 0.0001);
    // specular shading
#if BLINN == 1
    float3 halfwayDir = normalize(lightDir + toCam);
    float spec = pow(max(dot(norm, halfwayDir), 0.0001), 32.0f);
#else
    float3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(toCam, reflectDir), 0.0001), 16.0f);
#endif
    // attenuation
    float distance = length(light.position.xyz - fragWorldPos);
    float attenuation = 1.0 / (constant + lin * distance + quadratic * (distance * distance)) * light.intensity.xyz;
    // combine results
    float3 ambient = inFragTexCol * g_materialAmbient;
    float3 diffuse = diff * inFragTexCol * g_materialDiffuse;
    float3 specular = spec * inFragTexCol * g_materialSpecular;

    //! Attenuation is left unaffected by border intensity
    ambient *= attenuation;
    diffuse *= attenuation;
    specular *= attenuation;
    // Apply color
    return flashMask * (ambient + diffuse + specular);
}