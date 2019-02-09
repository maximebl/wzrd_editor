struct vertex_in
{
	float3 position : POSITION;
	float2 tex_coord : TEXCOORD;
};

struct vertex_out
{
	float4 position : SV_POSITION;
	float2 tex_coord : TEXCOORD;
};

Texture2D g_texture : register(t0);
SamplerState g_sampler : register(s0);

vertex_out VS(vertex_in vin)
{
	vertex_out vout;
    vout.position  = float4(vin.position , 1.0f);
	vout.tex_coord = vin.tex_coord;
	return vout;
}

float4 PS(vertex_out pin) : SV_Target
{
    //float2 test_var;
    //test_var.x = 0.0f;
    //test_var.y = 1.0f;
    return g_texture.Sample(g_sampler, pin.tex_coord);
    //return g_texture.Sample(g_sampler, test_var);
}