#ifndef GLOBALS_HLSLI
#define GLOBALS_HLSLI

sampler g_pointWrap : register(s0);
sampler g_linearWrap : register(s1);
sampler g_anisotropicWrap : register(s2);
sampler g_bilinearClamp : register(s4);

#endif // GLOBALS_HLSLI