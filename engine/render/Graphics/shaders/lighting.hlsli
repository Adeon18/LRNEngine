#include "globals.hlsli"
#include "utility.hlsli"

static const int MAX_POINTLIGHT_COUNT = 10;
static const int MAX_DIRLIGHT_COUNT = 1;

static const float MIN_LIGHT_INTENCITY = 0.0001f;

Texture2D g_textureSpotLight : TEXTURE : register(t16);

struct Material
{
    float3 ambient;
    float3 diffuse;
    float3 specular;

    float shininess;
};

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

float getDiffuseComponent(float3 lightDirection, float3 norm)
{
    return max(dot(norm, lightDirection), MIN_LIGHT_INTENCITY);
}

float getSpecularComponent(float3 lightDirection, float3 norm, float3 toCam, float3 shininess)
{
#if BLINN == 1
    float3 halfwayDir = normalize(lightDirection + toCam);
    return pow(max(dot(norm, halfwayDir), MIN_LIGHT_INTENCITY), shininess);
#else
    float3 reflectDir = reflect(-lightDirection, norm);
    return pow(max(dot(toCam, reflectDir), MIN_LIGHT_INTENCITY), shininess);
#endif
}

//! Get the attenuation based on the components and the distance
float getAttenuation(float constantC, float linearC, float quadraticC, float distance)
{
    return 1.0 / (constantC + linearC * distance + quadraticC * (distance * distance));
}

float3 calculateDirectionalLight(DirectionalLight light, Material mat, float3 norm, float3 toCam, float3 inFragTexCol)
{
    float3 lightDir = normalize(-light.direction.xyz);
    
    float3 diff = getDiffuseComponent(lightDir, norm);
    float spec = getSpecularComponent(lightDir, norm, toCam, mat.shininess);
    
    // combine results
    float3 ambient = mat.ambient;
    float3 diffuse = diff * mat.diffuse;
    float3 specular = spec * mat.specular;
    // Apply light color, intensity and surface color
    return inFragTexCol * light.intensity.xyz * light.color.xyz * (ambient + diffuse + specular);
}

// calculates the color when using a point light.
float3 calculatePointLight(PointLight light, Material mat, float3 norm, float3 fragWorldPos, float3 toCam, float3 inFragTexCol)
{    
    float3 lightDir = normalize(light.position.xyz - fragWorldPos);
    // diffuse shading
    float diff = getDiffuseComponent(lightDir, norm);
    // specular shading
    float spec = getSpecularComponent(lightDir, norm, toCam, mat.shininess);
    // attenuation
    float distance = length(light.position.xyz - fragWorldPos);
    float lightIntencity = getAttenuation(light.distProperties.x, light.distProperties.y, light.distProperties.z, distance) * light.intensity.xyz;
    
    // combine results
    float3 ambient = mat.ambient;
    float3 diffuse = diff * lightIntencity * mat.diffuse;
    float3 specular = spec * lightIntencity * mat.specular;

    // Apply light color and surface color
    return inFragTexCol * light.color.xyz * (ambient + diffuse + specular);
}

float3 calculateSpotLight(SpotLight light, Material mat, float3 norm, float3 fragWorldPos, float3 toCam, float3 inFragTexCol)
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
    
    if (theta < COS_CUTOFF_ANGLE)
    {
        return mat.ambient * inFragTexCol;
    }
        
    // diffuse shading
    float diff = getDiffuseComponent(lightDir, norm);
    // specular shading
    float spec = getSpecularComponent(lightDir, norm, toCam, mat.shininess);
    // attenuation
    float distance = length(light.position.xyz - fragWorldPos);
    float lightIntensity = getAttenuation(light.distProperties.x, light.distProperties.y, light.distProperties.z, distance) * light.intensity.xyz;
    // combine results
    float3 ambient = mat.ambient;
    float3 diffuse = diff * lightIntensity * mat.diffuse;
    float3 specular = spec * lightIntensity * mat.specular;

    // Apply mask color and the input texture color
    return inFragTexCol * flashMask * (ambient + diffuse + specular);
}