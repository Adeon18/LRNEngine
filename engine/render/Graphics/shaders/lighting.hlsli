#define MAX_POINTLIGHT_COUNT 10
#define MAX_DIRLIGHT_COUNT 1

struct DirectionalLight
{
    float4 direction;
    float4 ambient;
    float4 diffuse;
    float4 specular;
    
    float4 color;
};

struct PointLight
{
    float4 position;
    float4 ambient;
    float4 diffuse;
    float4 specular;
    
    // X - constant, Y - linear, Z - quadratic
    float4 distProperties;
    
    float4 color;
};

struct SpotLight
{
    float4 position;
    float4 direction;
    float4 cutoffAngleInnerCos;
    float4 cutoffAngleOuterCos;
    
    float4 ambient;
    float4 diffuse;
    float4 specular;
    
    // X - constant, Y - linear, Z - quadratic
    float4 distProperties;
    
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
    float3 ambient = light.ambient.xyz * inFragTexCol;
    float3 diffuse = light.diffuse.xyz * diff * inFragTexCol;
    float3 specular = light.specular.xyz * spec * inFragTexCol;
    // Apply color
    return light.color.xyz * (ambient + diffuse + specular);
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
    float attenuation = 1.0 / (constant + lin * distance + quadratic * (distance * distance));
    // combine results
    float3 ambient = light.ambient.xyz * inFragTexCol;
    float3 diffuse = light.diffuse.xyz * diff * inFragTexCol;
    float3 specular = light.specular.xyz * spec * inFragTexCol;

    ambient *= attenuation;
    diffuse *= attenuation;
    specular *= attenuation;
    // Apply color
    return light.color.xyz * (ambient + diffuse + specular);
}

float3 calculateSpotLight(SpotLight light, float3 norm, float3 fragWorldPos, float3 toCam, float3 inFragTexCol)
{
    float constant = light.distProperties.x;
    float lin = light.distProperties.y;
    float quadratic = light.distProperties.z;
    
    float3 lightDir = normalize(light.position.xyz - fragWorldPos);
    float theta = dot(lightDir, -light.direction.xyz);
    float epsilon = light.cutoffAngleInnerCos - light.cutoffAngleOuterCos;
    
    float borderIntensity = clamp((theta - light.cutoffAngleOuterCos.x) / epsilon, 0.0f, 1.0f);
    
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
    float attenuation = 1.0 / (constant + lin * distance + quadratic * (distance * distance));
    // combine results
    float3 ambient = light.ambient.xyz * inFragTexCol;
    float3 diffuse = light.diffuse.xyz * diff * inFragTexCol;
    float3 specular = light.specular.xyz * spec * inFragTexCol;

    //! Attenuation is left unaffected by border intensity
    ambient *= attenuation;
    diffuse *= attenuation * borderIntensity;
    specular *= attenuation * borderIntensity;
    // Apply color
    return light.color.xyz * (ambient + diffuse + specular);
}