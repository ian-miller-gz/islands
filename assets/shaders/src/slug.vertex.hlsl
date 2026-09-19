
struct Input
{
    float4 Position : TEXCOORD0;
    float4 Sample : TEXCOORD1;
    float4 Jacobian : TEXCOORD2;
    float4 Banding : TEXCOORD3;
    float4 Color : TEXCOORD4;
};

struct Output
{
    float4 Position : SV_Position;
    float4 Color : TEXCOORD0;
    float2 Coordinates : TEXCOORD1;
    nointerpolation float4 Banding : TEXCOORD2;
    nointerpolation int4 Glyph : TEXCOORD3;
};

cbuffer UniformBlock : register(b0, space1)
{
    float4x4 Matrix : packoffset(c0);
    float4 Viewport : packoffset(c4);
};

void SlugUnpack(float4 tex, float4 bnd, out float4 vbnd, out int4 vgly)
{
    uint2 g = asuint(tex.zw);
    vgly = int4(g.x & 0xFFFFU, g.x >> 16U, g.y & 0xFFFFU, g.y >> 16U);
    vbnd = bnd;
}

float2 SlugDilate(float4 pos, float4 tex, float4 jac, float4 m0, float4 m1, float4 m3, float2 dim, out float2 vpos)
{
    float2 n = normalize(pos.zw);
    float s = dot(m3.xy, pos.xy) + m3.w;
    float t = dot(m3.xy, n);

    float u = (s * dot(m0.xy, n) - t * (dot(m0.xy, pos.xy) + m0.w)) * dim.x;
    float v = (s * dot(m1.xy, n) - t * (dot(m1.xy, pos.xy) + m1.w)) * dim.y;

    float s2 = s * s;
    float st = s * t;
    float uv = u * u + v * v;
    float2 d = pos.zw * (s2 * (st + sqrt(uv)) / (uv - st * st));

    vpos = pos.xy + d;
    return (float2(tex.x + dot(d, jac.xy), tex.y + dot(d, jac.zw)));
}

Output main(Input input)
{
    float2 p;
    Output output;


    output.Coordinates = SlugDilate(input.Position, input.Sample,
        input.Jacobian, Matrix[0], Matrix[1], Matrix[3], Viewport.xy, p);


    output.Position = mul(Matrix, float4(p, 0.0, 1.0));


    SlugUnpack(input.Sample, input.Banding, output.Banding, output.Glyph);
    output.Color = input.Color;
    return output;
}
