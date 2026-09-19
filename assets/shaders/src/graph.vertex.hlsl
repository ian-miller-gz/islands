
cbuffer Camera : register(b0, space1)
{
    float4x4 Transform : packoffset(c0);
};

cbuffer Rotation : register(b1, space1)
{
    float4x4 Turn : packoffset(c0);
};

struct Input
{
    float3 Position : TEXCOORD0;
    float3 Normal : TEXCOORD1;
    float2 Coordinates : TEXCOORD2;
};

struct Output
{
    float3 Normal : TEXCOORD0;
    float2 Coordinates : TEXCOORD1;
    float4 Position : SV_Position;
};

Output main(Input input)
{
    Output output;
    output.Position = mul(Transform, float4(input.Position, 1.0f));
    output.Normal = normalize(mul((float3x3) Turn, input.Normal));
    output.Coordinates = input.Coordinates;
    return output;
}
