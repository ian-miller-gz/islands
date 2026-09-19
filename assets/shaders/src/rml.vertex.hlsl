
struct Input
{
    float2 Position : TEXCOORD0;
    float2 Texcoord : TEXCOORD1;
    float4 Color : TEXCOORD2;
};

struct Output
{
    float2 Texcoord : TEXCOORD0;
    float4 Color : TEXCOORD1;
    float4 Position : SV_Position;
};

cbuffer Camera : register(b0, space1)
{
    float4x4 ViewProjectionMatrix : packoffset(c0);
};

cbuffer Offset : register(b1, space1)
{
    float4 Translation : packoffset(c0);
};

Output main(Input input)
{
    float2 coord = input.Position + Translation.xy;

    Output output;
    output.Position = mul(ViewProjectionMatrix, float4(coord, 0.0f, 1.0f));
    output.Texcoord = input.Texcoord;
    output.Color = input.Color;
    return output;
}
