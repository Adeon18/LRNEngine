cbuffer perFrame : register(b0)
{
    float4x4 worldToClip;
    float4x4 worldToClipInv;
    float4 iResolution;
    float4 iCameraPosition;
    float iTime;
};

cbuffer perMesh : register(b1)
{
    float4x4 meshToModel;
    float4x4 meshToModelInv;
};

float distanceIntensity(float value, float target, float fade)
{
    return min(1.0, abs(value - target) / fade);
}

float periodIntensity(float value, float period, float fade)
{
    float target = round(value / period) * period;
    return distanceIntensity(value, target, fade);
}

float wave(float3 pos, float waveInterval, float waveYSpeed, float waveThickness, uniform bool distort)
{
    if (distort)
    {
        const float WAVE_XZ_SPEED = 3.0;
        const float WAVE_DISTORTION_SIZE = 0.035;
        const float WAVE_OSCILLATING_TIME = 4.0;

        float distortionSign = abs(frac(iTime / WAVE_OSCILLATING_TIME) - 0.5) * 4 - 1;
        float2 distortion = sin(pos.xz / WAVE_DISTORTION_SIZE + iTime * WAVE_XZ_SPEED) * WAVE_DISTORTION_SIZE * distortionSign;
        pos.y -= (distortion.x + distortion.y);
    }

    pos.y -= iTime * waveYSpeed;

    float intensity = 1.0 - periodIntensity(pos.y, waveInterval, waveThickness);
    return intensity;
}

// Note: in HLSL global constant is marked with "static const".
// One "const" is not enough, because it will be considered to be a uniform from a constant buffer.
// In HLSL const means that the value can not be changed by the shader.
// Adding "static" to global variable means that it is not visible to application, so doesn't belong to a constant buffer.
// A local constant inside a function can be marked just with "const".
// "static" for locals preserves value during current shader thread execution.

static const float BLUE_WAVE_INTERVAL = 10;
static const float BLUE_WAVE_SPEED = 10.0;
static const float BLUE_WAVE_THICKNESS = 1.0;

static const float RED_WAVE_INTERVAL = 100.0;
static const float RED_WAVE_SPEED = 20.0;
static const float RED_WAVE_THICKNESS = 5.0;

// called in vertex shader
float3 vertexDistortion(float3 pos, float3 normal)
{
    float3 offset = 0.0;
    offset += normal * 0.75 * wave(pos, BLUE_WAVE_INTERVAL, BLUE_WAVE_SPEED, BLUE_WAVE_THICKNESS, true);
    offset += normal * 2.0 * wave(pos, RED_WAVE_INTERVAL, RED_WAVE_SPEED, RED_WAVE_THICKNESS, false);
    return offset;
}

struct VS_INPUT
{
    float3 inPos : POSITION;
    float3 inNorm : NORMAL;
    float3 inTangent : TANGENT;
    float3 inBiTangent : BITANGENT;
    float2 inTC : TEXCOORD;
    float4 modelToWorld0 : MODEL2WORLD0;
    float4 modelToWorld1 : MODEL2WORLD1;
    float4 modelToWorld2 : MODEL2WORLD2;
    float4 modelToWorld3 : MODEL2WORLD3;
    float4 color : COLOR;
};

struct VS_OUTPUT
{
    float4 outPos : SV_POSITION;
    float3 modelPos : POS;
    float4 outCol : COLOR;
    float3 modelNorm : NORM;
};

VS_OUTPUT main(VS_INPUT input)
{
    VS_OUTPUT output;
    float4x4 modelToWorld = float4x4(input.modelToWorld0, input.modelToWorld1, input.modelToWorld2, input.modelToWorld3);
    
    
    float3 modelNorm = normalize(mul(float4(input.inNorm, 0.0f), transpose(meshToModelInv)));
    float4 modelPos = mul(float4(input.inPos, 1.0f), meshToModel);
    float3 offset = vertexDistortion(modelPos.xyz, modelNorm);
    modelPos += float4(offset, 1.0f);
    
    float4 worldPos = mul(modelPos, modelToWorld);
    
    output.outPos = mul(worldPos, worldToClip);
    output.modelPos = modelPos.xyz;
    output.outCol = input.color;
    output.modelNorm = modelNorm;
    return output;
}