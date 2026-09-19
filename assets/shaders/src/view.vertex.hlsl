
struct Output
{
    float2 Coordinates : TEXCOORD0;
    float4 Position : SV_Position;
};

static const float2 corners[3] = {
    {-1.0f, -1.0f},
    {3.0f, -1.0f},
    {-1.0f, 3.0f}
};

Output main(uint id : SV_VertexID)
{
    Output output;
    float2 corner = corners[id];
    output.Position = float4(corner, 0.0f, 1.0f);
    output.Coordinates = float2((corner.x + 1.0f) * 0.5f,
                                (1.0f - corner.y) * 0.5f);
    return output;
}
