cbuffer perFrame : register(b0)
{
    float4x4 worldToClip;
    float4x4 worldToClipInv;
    float4 iResolution;
    float4 iCameraPosition;
    float iTime;
};

cbuffer perFrame2 : register(b1)
{
    float4 cameraPosition;
};

// Frisvad with z == -1 problem avoidance
void basisFromDir(out float3 right, out float3 top, in float3 dir)
{
    float k = 1.0 / max(1.0 + dir.z, 0.00001);
    float a = dir.y * k;
    float b = dir.y * a;
    float c = -dir.x * a;
    right = float3(dir.z + b, c, -dir.x);
    top = float3(c, 1.0 - b, -dir.y);
}

// Frisvad with z == -1 problem avoidance
float3x3 basisFromDir(float3 dir)
{
    float3x3 rotation;
    rotation[2] = dir;
    basisFromDir(rotation[0], rotation[1], dir);
    return rotation;
}


struct VS_IN
{
    uint vertexId : SV_VertexID;
    float4 colorAndAlpha : COLOR;
    float3 centerPosition : POSITION;
    float3 velocity: VELOCITY;
    float2 size: SIZE;
    float axisRotation : ROTATION;
    float lifeTime: LIFE;
};

struct VS_OUTPUT
{
    float4 clipPos : SV_POSITION;
    float4 color : COLOR;
};

float3 getWorldPosFromID(uint vertexId, float2 size, float3 particlePos, float3 up, float3 right)
{
    if (vertexId == 0)
    {
        return particlePos - size.x / 2.0f * right - size.y / 2.0f * up;
    }
    else if (vertexId == 1)
    {
        return particlePos + size.x / 2.0f * right - size.y / 2.0f * up;
    }
    else if (vertexId == 2)
    {
        return particlePos - size.x / 2.0f * right + size.y / 2.0f * up;
    }
    else if (vertexId == 3)
    {
        return particlePos + size.x / 2.0f * right + size.y / 2.0f * up;
    }
    return float3(0, 0, 0);
}

VS_OUTPUT main(VS_IN input)
{
    VS_OUTPUT output;
    
    float3 upVector = float3(0.0f, 1.0f, 0.0f);
    float3 planeNormal = normalize(input.centerPosition - cameraPosition.xyz);
    float3 rightVector = normalize(cross(planeNormal, upVector));
    upVector = normalize(cross(rightVector, planeNormal));
    
    float3 worldPosFixed = getWorldPosFromID(input.vertexId, input.size, input.centerPosition, upVector, rightVector);
    
    float4 worldPos = float4(worldPosFixed, 1.0f);
    
    output.clipPos = mul(worldPos, worldToClip);
    output.color = input.colorAndAlpha;
    // Fill each vertex separately depending on vertex ID => CLOCKVISE
    
    return output;
}