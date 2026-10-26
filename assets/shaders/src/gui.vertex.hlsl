
struct Element
{
    float2 Position;
    float2 Extent;
    float4 Frame;
    float4 Color;
    float2 Axis;
    float Fringe;
    float Caps;
};

struct Output
{
    float2 Texcoord : TEXCOORD0;
    float4 Color : TEXCOORD1;
    float3 Ramp : TEXCOORD2;
    float2 Ends : TEXCOORD3;
    float4 Position : SV_Position;
};

static const float HALF = 0.5f;
static const float OPEN = 65536.0f;
static const uint NEAR = 1u;
static const uint FAR = 2u;

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
    float grown = element.Fringe * HALF;
    uint caps = (uint)element.Caps;
    float head = (caps & NEAR) != 0u ? grown : 0.0f;
    float tail = (caps & FAR) != 0u ? grown : 0.0f;
    float along = unit.x * (element.Extent.x + head + tail) - head;
    float across = unit.y * (element.Extent.y + 2.0f * grown) - grown;
    float2 normal = float2(-element.Axis.y, element.Axis.x);
    float2 coord = element.Position + element.Axis * along + normal * across;

    Output output;
    output.Position = mul(ViewProjectionMatrix, float4(coord, 0.0f, 1.0f));
    output.Texcoord = element.Frame.xy + unit * element.Frame.zw;
    output.Color = element.Color;
    float edge = element.Extent.y * HALF;
    output.Ramp = float3(across - edge, edge, element.Fringe);
    output.Ends = float2(
        head > 0.0f ? along : OPEN,
        tail > 0.0f ? element.Extent.x - along : OPEN);
    return output;
}
