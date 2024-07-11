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

cbuffer constant: register(b0)
{
    row_major float4x4 m_world;
    row_major float4x4 m_view;
    row_major float4x4 m_proj;
    unsigned int m_time;
};

vs_output main(vs_input input)
{
    vs_output output = (vs_output)0;
    //world space pos
    output.pos = mul(input.pos, m_world);
	//view space
    output.pos = mul(output.pos, m_view);
	//screen space
    output.pos = mul(output.pos, m_proj);
    //color
    output.coord = input.coord;
    return output;
}