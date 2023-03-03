struct DirectionalLight
{
    float4 direction;
    float4 ambient;
    float4 diffuse;
    float4 specular;
};

cbuffer perFrameLight : register(b1)
{
    DirectionalLight directLight;
};

float3 calculateDirectionalLight(DirectionalLight dirLight, float3 norm, float3 toCam, float3 inFragTexCol)
{
    float3 lightDir = normalize(-dirLight.direction.xyz);
    // diffuse shading
    float diff = max(dot(norm, lightDir), 0.0001);
    // specular shading
    float3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(toCam, reflectDir), 0.0001), 32.0f);
    // combine results
    float3 ambient = dirLight.ambient.xyz * inFragTexCol;
    float3 diffuse = dirLight.diffuse.xyz * diff * inFragTexCol;
    float3 specular = dirLight.specular.xyz * spec * inFragTexCol;
    return (ambient + diffuse + specular);
    //return ambient;
}