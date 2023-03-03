#define MAX_POINTLIGHT_COUNT 10

struct DirectionalLight
{
    float4 direction;
    float4 ambient;
    float4 diffuse;
    float4 specular;
};

struct PointLight
{
    float4 position;
    float4 ambient;
    float4 diffuse;
    float4 specular;
    
    // X - constant, Y - linear, Z - quadratic
    float4 distProperties;
};


cbuffer perFrameLight : register(b1)
{
    // Has 4 ints that describe point light count
    int4 pointLightCount;
    DirectionalLight directLight;
    PointLight pointLights[MAX_POINTLIGHT_COUNT];
};

#define BLINN 1

float3 calculateDirectionalLight(DirectionalLight dirLight, float3 norm, float3 toCam, float3 inFragTexCol)
{
    float3 lightDir = normalize(-dirLight.direction.xyz);
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
    float3 ambient = dirLight.ambient.xyz * inFragTexCol;
    float3 diffuse = dirLight.diffuse.xyz * diff * inFragTexCol;
    float3 specular = dirLight.specular.xyz * spec * inFragTexCol;
    return (ambient + diffuse + specular);
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
    return (ambient + diffuse + specular);
}