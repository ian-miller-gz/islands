
Texture2D<float4> Texture : register(t0, space2);
SamplerState Sampler : register(s0, space2);

static const int NODES = 16;

static const float COLOR = 0.0f;
static const float SAMPLE = 1.0f;
static const float BLEND = 2.0f;
static const float NOISE = 3.0f;
static const float RAMP = 4.0f;
static const float LIGHT = 5.0f;

struct Node
{
    float Kind;
    float One;
    float Two;
    float Amount;
    float4 Value;
};

cbuffer Graph : register(b0, space3)
{
    float Count : packoffset(c0.x);
    float Surface : packoffset(c0.y);
    float2 Reserved : packoffset(c0.z);
    Node Nodes[NODES] : packoffset(c1);
};

struct Input
{
    float3 Normal : TEXCOORD0;
    float2 Coordinates : TEXCOORD1;
};

float hash(float2 spot)
{
    return frac(sin(dot(spot, float2(127.1f, 311.7f))) * 43758.545f);
}

float noise(float2 spot)
{
    float2 cell = floor(spot);
    float2 part = frac(spot);
    float2 blend = part * part * (3.0f - 2.0f * part);
    float a = hash(cell);
    float b = hash(cell + float2(1.0f, 0.0f));
    float c = hash(cell + float2(0.0f, 1.0f));
    float d = hash(cell + float2(1.0f, 1.0f));
    return lerp(lerp(a, b, blend.x), lerp(c, d, blend.x), blend.y);
}

float4 main(Input input) : SV_Target0
{
    float4 results[NODES];
    for (int seed = 0; seed < NODES; seed += 1)
        results[seed] = float4(0.0f, 0.0f, 0.0f, 1.0f);
    int count = min((int) Count, NODES);
    for (int index = 0; index < count; index += 1)
    {
        Node node = Nodes[index];
        float4 one = results[(int) node.One];
        float4 two = results[(int) node.Two];
        float4 result = node.Value;
        if (node.Kind == SAMPLE)
            result = Texture.Sample(Sampler, input.Coordinates) * node.Value;
        else if (node.Kind == BLEND)
            result = lerp(one, two, node.Amount);
        else if (node.Kind == NOISE)
        {
            float gray = noise(input.Coordinates * node.Amount);
            result = float4(gray, gray, gray, 1.0f);
        }
        else if (node.Kind == RAMP)
            result = lerp(one, two, saturate(input.Coordinates.y + node.Amount));
        else if (node.Kind == LIGHT)
        {
            float facing =
                max(dot(normalize(input.Normal), -normalize(node.Value.xyz)),
                    0.0f);
            result = float4(one.rgb * saturate(facing + node.Amount), one.a);
        }
        results[index] = result;
    }
    return results[(int) Surface];
}
