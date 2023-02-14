#include "HologramStructs.hlsli"

[outputcontrolpoints(3)] // doesn’t have to match InputPatch argument size (3)
[domain("tri")]
[outputtopology("triangle_cw")]
[partitioning("integer")]
[patchconstantfunc("mainPatch")]
VS_OUTPUT main(InputPatch<VS_OUTPUT, 3> input,
	uint pointId : SV_OutputControlPointID,
	uint patchId : SV_PrimitiveID)
{
    return input[pointId]; // input size matches the number specified in the set PATCHLIST Topology
}

#define WORLD_SPACE_TESSELATION_OFF 0

//! Factor by which we scale the endge to determine the scaling factor
static const int EDGE_LEN_SCALING_FACTOR = 5;


PATCH_OUTPUT mainPatch(InputPatch<VS_OUTPUT, 3> input, uint patchId : SV_PrimitiveID)
{
    PATCH_OUTPUT output;
   
    float e0Length = length(input[0].worldPos - input[1].worldPos);
    float e1Length = length(input[1].worldPos - input[2].worldPos);
    float e2Length = length(input[2].worldPos - input[0].worldPos);
    
    float avgEdgeLen = (e0Length + e1Length + e2Length) / 3.0f;
    
    int innerFactor = avgEdgeLen * EDGE_LEN_SCALING_FACTOR;
    
#if WORLD_SPACE_TESSELATION_OFF
    output.EdgeFactors[0] = 1;
    output.EdgeFactors[1] = 1;
    output.EdgeFactors[2] = 1;
    output.InsideFactor = 1;
#else 
    // The minimum factor value must be 1 else the triangle won't be drawn
    output.EdgeFactors[0] = max(e0Length * EDGE_LEN_SCALING_FACTOR, 1);
    output.EdgeFactors[1] = max(e1Length * EDGE_LEN_SCALING_FACTOR, 1);
    output.EdgeFactors[2] = max(e2Length * EDGE_LEN_SCALING_FACTOR, 1);
    output.InsideFactor = max(innerFactor, 1);
#endif
    return output;
}