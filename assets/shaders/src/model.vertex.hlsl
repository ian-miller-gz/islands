
struct Input
{
    float3 Position : TEXCOORD0;
    float3 Normal : TEXCOORD1;
    float2 Coordinates : TEXCOORD2;
};

struct Output
{
    float2 Coordinates : TEXCOORD0;
    float3 Normal : TEXCOORD1;
    float4 Position : SV_Position;
};

cbuffer UniformBlock : register(b0, space1)
{
    float4x4 ViewProjectionMatrix : packoffset(c0);
};

Output main(Input input)
{
    Output output;
    output.Position = mul(ViewProjectionMatrix, float4(input.Position, 1.0f));
    output.Coordinates = input.Coordinates;
    output.Normal = input.Normal;
    return output;
}
