Texture2D Texture : register(t0);
sampler TextureSampler : register(s0);

struct ps_input
{
    float4 pos : SV_POSITION;
    float2 coord : TEXCOORD0;
    float3 norm : NORMAL0;
    float3 direction_to_camera : TEXCOORD1;
};

float4 main(ps_input input) : SV_TARGET
{
    return Texture.Sample(TextureSampler, 1.0f - input.coord);
}