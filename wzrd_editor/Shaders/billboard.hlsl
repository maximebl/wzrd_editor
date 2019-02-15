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

[maxvertexcount(3)]
void GS(
    point vs_out gin[1],
    inout TriangleStream<gs_out> tri_stream
)
{
    gs_out geo_out;

    geo_out.position.x = 0.0f;
    geo_out.position.y = 0.0f;

    geo_out.position.z = 0.0f;
    geo_out.position.w = 1.0f;

    geo_out.tex_coord.x = gin[0].tex_coord.x;
    geo_out.tex_coord.y = gin[0].tex_coord.y;

    for (int i = 0; i < 3; i++)
    {
        if(i == 0)
        {
            geo_out.position.x = 0.f;
            geo_out.position.y = 0.f;
            tri_stream.Append(geo_out);
            continue;
        }

        if (i == 1)
        {
            geo_out.position.x = 0.3f;
            geo_out.position.y = 0.f;
            tri_stream.Append(geo_out);
            continue;
        }

        if (i == 2)
        {
            geo_out.position.x = 0.3f;
            geo_out.position.y = 0.3;
            tri_stream.Append(geo_out);
            continue;
        }
    }
}

float4 PS(gs_out pin) : SV_Target
{
    //return g_texture.Sample(g_sampler, pin.tex_coord);
    return float4(1.f, 1.f, 0.f, 0.f);
}