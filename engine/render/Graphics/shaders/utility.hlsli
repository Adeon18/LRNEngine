#ifndef UTILITY_HLSLI
#define UTILITY_HLSLI

float remap(float low1, float high1, float low2, float high2, float value)
{
    return low2 + (value - low1) * (high2 - low2) / (high1 - low1);
}

#endif // UTILITY_HLSLI