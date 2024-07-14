Texture2D Texture : register(t0);
sampler TextureSampler : register(s0);

struct ps_input
{
    float4 pos : SV_POSITION;
    float2 coord : TEXCOORD0;
    float3 norm : NORMAL0;
    float3 direction_to_camera : TEXCOORD1;
};

cbuffer constant : register(b0)
{
    row_major float4x4 m_world;
    row_major float4x4 m_view;
    row_major float4x4 m_proj;
    float4 m_light_direction;
};

float4 main(ps_input input) : SV_TARGET
{
    //ambient light
    float ka = 0.1f; //constant
    float3 ia = float3(1.0f,1.0f,1.0f); //light color
    float3 ambient_light = ka * ia;
    //diffuse light
    float kd = 1.0f; //constant
    float3 id = float3(1.0f, 1.0f, 1.0f); //light color
    //using max function with the 0.0 float to get only positive values of the dot product
    float amount_diffuse_light = max(0.0f, dot(m_light_direction.xyz, input.norm));
    float3 diffuse_light = kd * amount_diffuse_light * id;
    //specular light
    float ks = 0.7; //constant
    float3 is = float3(1.0, 1.0, 1.0); //light color
    float3 reflected_light = reflect(m_light_direction.xyz, input.norm); //reflection
    float shininess = 100.0; //alpha value
    float amount_specular_light = pow(max(0.0, dot(reflected_light, input.direction_to_camera)), shininess);
    float3 specular_light = ks * amount_specular_light * is;
    //final light
    float3 final_light = ambient_light + diffuse_light + specular_light;
    
    return float4(final_light,1.0f);
}