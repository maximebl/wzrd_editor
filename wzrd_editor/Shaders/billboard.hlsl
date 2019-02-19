cbuffer cb_texcoords : register(b0)
{
    float2 topleft;
    float2 topright;
    float2 bottomleft;
    float2 bottomright;
};

struct vs_in
{
    float3 position : POSITION;
    float2 tex_coord : TEXCOORD;
};

struct vs_out
{
    float4 position : SV_POSITION;
    float2 tex_coord : TEXCOORD;
};

struct gs_out
{
    float4 position : SV_POSITION;
    float2 tex_coord : TEXCOORD;
};

Texture2D g_texture : register(t0);
SamplerState g_sampler : register(s0);

vs_out VS(vs_in vin)
{
    vs_out vout;
    vout.position = float4(vin.position, 1.0f);
    vout.tex_coord = vin.tex_coord;
    return vout;
}

[maxvertexcount(4)]
void GS(
    point vs_out gin[1],
    inout TriangleStream<gs_out> tri_stream
)
{
    gs_out geo_out;

    geo_out.position.z = 0.0f;
    geo_out.position.w = 1.0f;

// top left
    geo_out.position.x = -0.3f;
    geo_out.position.y = 0.3f;
    geo_out.tex_coord.x = topleft.x;
    geo_out.tex_coord.y = topleft.y;
    tri_stream.Append(geo_out);
// top right
    geo_out.position.x = 0.3f;
    geo_out.position.y = 0.3f;
    geo_out.tex_coord.x = topright.x;
    geo_out.tex_coord.y = topright.y;;
    tri_stream.Append(geo_out);
// bottom left
    geo_out.position.x = -0.3f;
    geo_out.position.y = -0.3;
    geo_out.tex_coord.x = bottomleft.x;
    geo_out.tex_coord.y = bottomleft.y;;
    tri_stream.Append(geo_out);
// bottom right
    geo_out.position.x = 0.3f;
    geo_out.position.y = -0.3;
    geo_out.tex_coord.x = bottomright.x;
    geo_out.tex_coord.y = bottomright.y;
    tri_stream.Append(geo_out);
}

float4 PS(gs_out pin) : SV_Target
{
    return g_texture.Sample(g_sampler, pin.tex_coord);
}