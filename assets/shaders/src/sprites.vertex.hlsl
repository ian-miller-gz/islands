
struct Sprite
{
    float3 Position;
    float Rotation;
    float2 Scale;
    float2 Padding;
    float TexU, TexV, TexW, TexH;
    float4 Color;
};

struct Output
{
    float2 Texcoord : TEXCOORD0;
    float4 Color : TEXCOORD1;
    float4 Position : SV_Position;
};

StructuredBuffer<Sprite> sprites : register(t0, space0);

cbuffer Uniform : register(b0, space1)
{
    float4x4 ViewProjectionMatrix : packoffset(c0);
};

static const uint TRIANGLE_INDICES[6] = {0, 1, 2, 3, 2, 1};
static const float2 VERTEX_POSITION[4] = {
    {0.0f, 0.0f},
    {1.0f, 0.0f},
    {0.0f, 1.0f},
    {1.0f, 1.0f}
};

Output main(uint id : SV_VertexID)
{
    uint index = id / 6;
    uint vertex = TRIANGLE_INDICES[id % 6];
    Sprite sprite = sprites[index];

    float2 texcoord[4] = {
        {sprite.TexU,               sprite.TexV              },
        {sprite.TexU + sprite.TexW, sprite.TexV              },
        {sprite.TexU,               sprite.TexV + sprite.TexH},
        {sprite.TexU + sprite.TexW, sprite.TexV + sprite.TexH}
    };

    float c = cos(sprite.Rotation);
    float s = sin(sprite.Rotation);

    float2 coord = VERTEX_POSITION[vertex];
    coord *= sprite.Scale;
    float2x2 rotation = {c, s, -s, c};
    coord = mul(coord, rotation);

    float3 position = float3(coord + sprite.Position.xy, sprite.Position.z);

    Output output;
    output.Position = mul(ViewProjectionMatrix, float4(position, 1.0f));
    output.Texcoord = texcoord[vertex];
    output.Color = sprite.Color;
    return output;
}
