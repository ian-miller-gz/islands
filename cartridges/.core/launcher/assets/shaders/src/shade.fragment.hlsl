
cbuffer Idle : register(b0, space3) {
  float time;
  float aspect;
  float pad0;
  float pad1;
};

static const float LAYERS = 6.0;
static const float TRIP = 48.0;
static const float FAR = 26.0;
static const float NEAR = 4.0;
static const float ROWS = 180.0;
static const float TAU = 6.2831853;

static const float3 DUSK = float3(0.010, 0.018, 0.052);
static const float3 PURPLE = float3(0.62, 0.32, 1.0);
static const float3 PINK = float3(1.0, 0.36, 0.72);

float2 toss(float2 seed) {
  const float2 mixed = float2(
    dot(seed, float2(127.1, 311.7)), dot(seed, float2(269.5, 183.3)));
  return frac(sin(mixed) * 43758.5453);
}

float3 stars(float2 plane, float2 shift, float cells, float fade) {
  const float2 grid = plane * cells + shift;
  const float2 seat = floor(grid);
  const float2 spot = toss(seat);
  const float2 traits = toss(seat + 71.3);
  const float2 toward = frac(grid) - lerp(0.15, 0.85, spot);
  const float span = dot(toward, toward);
  const float size = lerp(0.0004, 0.0032, traits.x * traits.x);
  const float wink = 0.75 + 0.25 * sin(time * 2.0 + traits.y * TAU);
  const float3 tint = lerp(PURPLE, PINK, traits.y);
  return tint * (size * wink * fade / (span + 0.0008));
}

float4 main(float4 position : SV_Position, float2 uv : TEXCOORD0)
  : SV_Target {
  const float2 flat =
    float2((uv.x - 0.5) * aspect, uv.y - 0.5) +
    float2(time * 0.0012, time * 0.0005);

  const float2 chunk = float2(ROWS * aspect, ROWS);
  const float2 coarse = (floor(uv * chunk) + 0.5) / chunk;
  const float2 deep = float2((coarse.x - 0.5) * aspect, coarse.y - 0.5) +
                      float2(time * 0.0012, time * 0.0005);

  float3 color = DUSK * (0.35 + 0.65 * uv.y);
  const float cloud =
    (0.5 + 0.5 * cos(flat.x * 2.3 + time * 0.021)) *
    (0.5 + 0.5 * cos(flat.y * 3.1 - time * 0.013));
  const float3 glow = lerp(PURPLE, PINK, 0.5 + 0.5 * sin(time * 0.007));
  color += glow * cloud * 0.016;

  for (float rung = 0.0; rung < LAYERS; rung += 1.0) {
    const float phase = frac(time / TRIP + rung / LAYERS);
    const float cells = lerp(FAR, NEAR, phase * phase);
    const float fade =
      smoothstep(0.0, 0.2, phase) * smoothstep(1.0, 0.85, phase);
    const float2 shift = toss(float2(rung, rung + 3.7)) * 61.0;
    color += stars(deep, shift, cells, fade);
  }
  return float4(color, 1.0);
}
