struct vs_input
{
    float4 pos : POSITION0;
    float2 coord : TEXCOORD0;
    float3 norm : NORMAL0;
};

struct vs_output
{
    float4 pos : SV_POSITION;
    float2 coord : TEXCOORD0;
    float3 norm : NORMAL0;
    float3 direction_to_camera : TEXCOORD1;
};

cbuffer constant: register(b0)
{
    row_major float4x4 m_world;
    row_major float4x4 m_view;
    row_major float4x4 m_proj;
    float4 m_light_direction;
    float4 m_camera_position;
};

vs_output main(vs_input input)
{
    vs_output output = (vs_output)0;
    //world space pos
    output.pos = mul(input.pos, m_world);
    output.direction_to_camera = normalize(output.pos.xyz - m_camera_position.xyz);
	//view space
    output.pos = mul(output.pos, m_view);
	//screen space
    output.pos = mul(output.pos, m_proj);
    //color
    output.coord = input.coord;
    output.norm = input.norm;
    return output;
}