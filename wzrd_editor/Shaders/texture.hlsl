cbuffer object_constants : register(b0)
{
	float4x4 world_view_proj;
};

struct vertex_in
{
	float3 position : POSITION;
	float2 tex_coord : TEXCOORD;
};

struct vertex_out
{
	float4 position_h : SV_POSITION;
	float2 tex_coord : TEXCOORD;
};

Texture2D g_texture : register(t0);
SamplerState g_sampler : register(s0);

vertex_out VS(vertex_in vin)
{
	vertex_out vout;
	vout.position_h = mul(float4(vin.position, 1.0f), world_view_proj);
	vout.tex_coord = vin.tex_coord;
	return vout;
}

float4 PS(vertex_out pin) : SV_Target
{
	//float3 result = g_texture.Sample(g_sampler, pin.tex_coord);
	return float4(pin.tex_coord, 0.0f, 0.0f);
}