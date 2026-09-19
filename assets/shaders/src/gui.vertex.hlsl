
struct Element
{
    float2 Position;
    float2 Extent;
    float4 Frame;
    float4 Color;
};

struct Output
{
    float2 Texcoord : TEXCOORD0;
    float4 Color : TEXCOORD1;
    float4 Position : SV_Position;
};

StructuredBuffer<Element> elements : register(t0, space0);

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
    Element element = elements[index];

    float2 unit = VERTEX_POSITION[vertex];
    float2 coord = unit * element.Extent + element.Position;

    Output output;
    output.Position = mul(ViewProjectionMatrix, float4(coord, 0.0f, 1.0f));
    output.Texcoord = element.Frame.xy + unit * element.Frame.zw;
    output.Color = element.Color;
    return output;
}
