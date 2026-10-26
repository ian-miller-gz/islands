
Texture2D<float4> Texture : register(t0, space2);
SamplerState Sampler : register(s0, space2);

struct Input
{
    float2 Texcoord : TEXCOORD0;
    float4 Color : TEXCOORD1;
    float3 Ramp : TEXCOORD2;
    float2 Ends : TEXCOORD3;
};

static const float HALF = 0.5f;

float4 main(Input input) : SV_Target0
{
    float4 color = input.Color * Texture.Sample(Sampler, input.Texcoord);
    if (input.Ramp.z > 0.0f)
    {
        float side = input.Ramp.y - abs(input.Ramp.x);
        float end = min(input.Ends.x, input.Ends.y);
        color.a *= saturate(min(side, end) / input.Ramp.z + HALF);
    }
    return color;
}
