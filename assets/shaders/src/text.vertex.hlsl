
struct Input
{
    float2 Position : TEXCOORD0;
    float2 Coordinates : TEXCOORD1;
    float4 Color : TEXCOORD2;
};

struct Output
{
    float2 Coordinates : TEXCOORD0;
    float4 Color : TEXCOORD1;
    float4 Position : SV_Position;
};

Output main(Input input)
{
    Output output;
    output.Position = float4(input.Position, 0.0f, 1.0f);
    output.Coordinates = input.Coordinates;
    output.Color = input.Color;
    return output;
}
