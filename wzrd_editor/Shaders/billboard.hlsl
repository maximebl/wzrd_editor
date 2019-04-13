cbuffer cb_texcoords : register(b0)
{
    float2 topleft;
    float2 topright;
    float2 bottomleft;
    float2 bottomright;
};

cbuffer cb_pos : register(b1)
{
    float2 pos_topleft;
    float2 pos_topright;
    float2 pos_bottomleft;
    float2 pos_bottomright;
};

cbuffer cb_manips : register(b2)
{
    float scale;
    uint filter_reduction;
    float forced_miplevel;
    float comparison_value;
    uint is_forced_mip_level;
    uint sampling_function;
    uint is_screen_coord;
    int x_pixel_offset;
    int y_pixel_offset;
};

struct readback_data
{
    float level_of_detail;
    uint width;
    uint height;
};

RWStructuredBuffer<readback_data> rwb_readback_data : register(u1);

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
Texture2D g_texture2 : register(t1);
SamplerState g_sampler : register(s0);
SamplerComparisonState g_cmp_sampler : register(s1);

float3 average(float2 texcoord)
{
    float3 result = float3(0.f, 0.0f, 0.0f);

    float4 reds = g_texture.GatherRed(g_sampler, texcoord, int2(x_pixel_offset, y_pixel_offset));
    float r1 = reds.x;
    float r2 = reds.y;
    float r3 = reds.z;
    float r4 = reds.w;
    float final_red = (r1 + r2 + r3 + r4) / 4;
    result.x = final_red;

    float4 greens = g_texture.GatherGreen(g_sampler, texcoord, int2(x_pixel_offset, y_pixel_offset));
    float g1 = greens.x;
    float g2 = greens.y;
    float g3 = greens.z;
    float g4 = greens.w;
    float final_green = (g1 + g2 + g3 + g4) / 4;
    result.y = final_green;

    float4 blues = g_texture.GatherBlue(g_sampler, texcoord, int2(x_pixel_offset, y_pixel_offset));
    float b1 = blues.x;
    float b2 = blues.y;
    float b3 = blues.z;
    float b4 = blues.w;
    float final_blue = (b1 + b2 + b3 + b4) / 4;
    result.z = final_blue;

    return result;
}

float3 bilinear(float2 texcoord, float tex_dimension)
{

    float3 result;

    // red channel
    float4 reds = g_texture.GatherRed(g_sampler, texcoord, int2(x_pixel_offset, y_pixel_offset));
    float r1 = reds.x;
    float r2 = reds.y;
    float r3 = reds.z;
    float r4 = reds.w;

    float2 pixel = texcoord * tex_dimension + 0.5;
    float2 fract = frac(pixel);

    float top_row_red = lerp(r4, r3, fract.x);
    float bottom_row_red = lerp(r1, r2, fract.x);

    float final_red = lerp(top_row_red, bottom_row_red, fract.y);
    result.x = final_red;
            
    // green channel
    float4 greens = g_texture.GatherGreen(g_sampler, texcoord, int2(x_pixel_offset, y_pixel_offset));
    float g1 = greens.x;
    float g2 = greens.y;
    float g3 = greens.z;
    float g4 = greens.w;

    float top_row_green = lerp(g4, g3, fract.x);
    float bottom_row_green = lerp(g1, g2, fract.x);

    float final_green = lerp(top_row_green, bottom_row_green, fract.y);
    result.y = final_green;
            
    // blue channel
    float4 blues = g_texture.GatherBlue(g_sampler, texcoord, int2(x_pixel_offset, y_pixel_offset));
    float b1 = blues.x;
    float b2 = blues.y;
    float b3 = blues.z;
    float b4 = blues.w;

    float top_row_blue = lerp(b4, b3, fract.x);
    float bottom_row_blue = lerp(b1, b2, fract.x);

    float final_blue = lerp(top_row_blue, bottom_row_blue, fract.y);
    result.z = final_blue;

    return result;
}

vs_out VS(vs_in vin)
{
    vs_out vout;
    vout.position = float4(vin.position, 1.0f);
    vout.tex_coord = vin.tex_coord;
    return vout;
}

[maxvertexcount(4)]
void GS(point vs_out gin[1], inout TriangleStream<gs_out> tri_stream)
{
    uint width_texels;
    uint height_texels;
    g_texture.GetDimensions(width_texels, height_texels);

    gs_out geo_out;
    geo_out.position.z = 0.0f;
    geo_out.position.w = 1.0f;

// top left
    geo_out.position.x = scale * pos_topleft.x;
    geo_out.position.y = scale * pos_topleft.y;
    geo_out.tex_coord.x = topleft.x;
    geo_out.tex_coord.y = topleft.y;
    tri_stream.Append(geo_out);

// top right
    geo_out.position.x = scale * pos_topright.x;
    geo_out.position.y = scale * pos_topright.y;
    geo_out.tex_coord.x = topright.x;
    geo_out.tex_coord.y = topright.y;
    tri_stream.Append(geo_out);

// bottom left
    geo_out.position.x = scale * pos_bottomleft.x;
    geo_out.position.y = scale * pos_bottomleft.y;
    geo_out.tex_coord.x = bottomleft.x;
    geo_out.tex_coord.y = bottomleft.y;
    tri_stream.Append(geo_out);

// bottom right
    geo_out.position.x = scale * pos_bottomright.x;
    geo_out.position.y = scale * pos_bottomright.y;
    geo_out.tex_coord.x = bottomright.x;
    geo_out.tex_coord.y = bottomright.y;
    tri_stream.Append(geo_out);
}

float4 PS(float4 screen_pos : SV_Position, gs_out pin) : SV_Target
{
    uint width_texels;
    uint height_texels;
    g_texture.GetDimensions(width_texels, height_texels);
    rwb_readback_data[0].width = width_texels;
    rwb_readback_data[0].height = height_texels;

    float4 result = float4(0.0f, 0.0f, 0.0f, 1.0f);
    float lod = 0.0f;

    if (is_screen_coord)
    {
        pin.tex_coord = screen_pos.xy;
    }

    if (is_forced_mip_level)
    {
        result = g_texture.SampleLevel(g_sampler, pin.tex_coord, forced_miplevel);
    }
    else
    {
        [branch]
        switch (sampling_function)
        {
            //sample function
            case 0:
                [branch]
                switch (filter_reduction)
                {
                    //standard
                    case 0:
                    //minimum
                    case 2:
                    //maximum
                    case 3:
                        result = g_texture.Sample(g_sampler, pin.tex_coord);
                        lod = g_texture.CalculateLevelOfDetail(g_sampler, pin.tex_coord);
                        rwb_readback_data[0].level_of_detail = lod;
                        break;
                    //comparison
                    case 1:
                        result = g_texture.SampleCmp(g_cmp_sampler, pin.tex_coord, comparison_value);
                        break;
                }
                break;

            //load function
            case 1:
                result = g_texture.Load(int3(screen_pos.xy, floor(forced_miplevel)));
                break;

            //gather red
            case 2:
                result = g_texture.GatherRed(g_sampler, pin.tex_coord);
                break;
            //gather green
            case 3:
                result = g_texture.GatherGreen(g_sampler, pin.tex_coord);
                break;
            //gather blue
            case 4:
                result = g_texture.GatherBlue(g_sampler, pin.tex_coord);
                break;
            //gather alpha
            case 5:
                result = g_texture.GatherAlpha(g_sampler, pin.tex_coord);
                break;

            //sample_grad
            case 6:
                result.xyz = average(pin.tex_coord);
                break;

            //sample_bias
            case 7:
                result.xyz = bilinear(pin.tex_coord, width_texels);
                break;
        }
    }
    return result;
}