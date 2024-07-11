struct vs_input
{
    float4 pos : POSITION0;
    float2 coord : TEXCOORD0;
};

struct vs_output
{
    float4 pos : SV_POSITION;
    float2 coord : TEXCOORD0;
};


vs_output main(vs_input input)
{
    vs_output output = (vs_output)0;
    return output;
}