
struct Input
{
    float3 Normal : TEXCOORD0;
};

cbuffer UniformBlock : register(b0, space3)
{
    float4 Color : packoffset(c0);
    float4 Factors : packoffset(c1);
};

static const float3 Light = normalize(float3(0.4f, 0.7f, 0.6f));
static const float3 View = float3(0.0f, 0.0f, 1.0f);
static const float3 Dielectric = float3(0.04f, 0.04f, 0.04f);
static const float Floor = 0.25f;

float4 main(Input input) : SV_Target0
{
    float3 normal = normalize(input.Normal);
    float diffuse = max(0.0f, dot(normal, Light));
    float gloss = exp2(1.0f + 10.0f * (1.0f - Factors.y));
    float shine = pow(max(0.0f, dot(normal, normalize(Light + View))), gloss);
    float3 base = Color.rgb * (1.0f - Factors.x);
    float3 metal = lerp(Dielectric, Color.rgb, Factors.x);
    float3 lit = base * (Floor + (1.0f - Floor) * diffuse) + metal * shine;
    return float4(lit, Color.a);
}
