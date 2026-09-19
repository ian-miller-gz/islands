
cbuffer Idle : register(b0, space3) {
  float time;
  float aspect;
  float pad0;
  float pad1;
};

static const float ROW = 1.7320508;
static const float NEAR = 25.0;
static const float FAR = 85.0;
static const float PASSES = 8.0;
static const float SWELL = 0.0015;
static const float SWAY = 0.15;
static const float PACE = 1.2;
static const float CRAWL = 0.22;
static const float CREEP = 0.07;

float beat() { return time * PACE; }

float2 wrap(float2 value, float2 span) {
  return value - span * floor(value / span);
}

float fade(float value, float span) {
  return smoothstep(0.0, span, span - value);
}

float reach(float2 local) {
  const float2 mark = abs(local);
  const float corner = dot(mark, normalize(float2(1.0, ROW)));
  return max(corner, mark.x);
}

float2 nearest(float2 plane) {
  const float2 span = float2(1.0, ROW);
  const float2 middle = span * 0.5;
  const float2 even = wrap(plane, span) - middle;
  const float2 odd = wrap(plane - middle, span) - middle;
  return dot(even, even) < dot(odd, odd) ? even : odd;
}

float2 turn(float2 plane, float angle) {
  const float sine = sin(angle);
  const float cosine = cos(angle);
  return mul(float2x2(cosine, sine, -sine, cosine), plane);
}

float rim(float2 grid) {
  const float edge = reach(nearest(grid));
  return smoothstep(0.45, 0.51, edge) - smoothstep(0.48, 0.51, edge);
}

float2 ripple(float2 plane) {
  const float phase = beat() * 0.5;
  for (float rung = 1.0; rung < PASSES; rung += 1.0) {
    const float span = rung * rung;
    const float2 wave = sin(plane * span + phase);
    plane.y += SWELL * wave.x * wave.y;
    plane.x += SWELL * sin(plane.y * span + phase) * wave.x;
  }
  return plane;
}

float sway(float period, float lift) {
  return (sin(beat() / period) * lift + lift) * SWAY;
}

float2 lamp() {
  return float2(0.5 + sin(beat() * 0.2) * 0.9, 0.5 + cos(beat() * 0.8) * 0.1);
}

float3 sheet(float2 plane, float falloff) {
  const float3 base = float3(0.9, 0.92, 0.055) + sin(plane.y * 7.0) * 0.015;
  const float2 toward = lamp() - plane;
  const float band = fade(abs(0.015 * toward.x + toward.y), 0.8);
  const float3 tint = lerp(float3(1.0, 1.0, 1.0), base * 2.0, 0.4);
  const float3 lit =
    base * 0.4 + base * falloff * 0.3 + tint * band * falloff * 0.8;
  return pow(lit, 1.0 / 2.2);
}

float4 main(float4 position : SV_Position, float2 uv : TEXCOORD0)
  : SV_Target {
  const float2 flat = float2((uv.x - 0.5) * aspect, uv.y - 0.5);
  const float2 plane = ripple(flat);

  const float3 axes = float3(plane.x, plane.y, plane.x);
  float3 color = 0.5 + 0.5 * cos(beat() / 100.0 + axes + float3(9.0, 5.0, 5.0));

  const float falloff = fade(length(lamp() - plane), 1.8);
  const float breath = sin(beat() / 150.0);
  const float3 coarse = color * float3(0.7, 0.95, 0.05) * falloff * 12.8;
  const float3 filter = float3(1.4, 0.45 + 0.35 * sin(beat() / 37.0),
                               0.45 + 0.35 * cos(beat() / 29.0));
  const float3 fine = color * filter * (10.0 / (breath + 1.5));

  color = lerp(color, sheet(plane, falloff), 0.5);
  const float2 walk = float2(beat(), 0.0);
  const float2 near = turn(plane * NEAR, sway(10.0, 0.15)) + walk * CRAWL;
  const float2 far = turn(flat * FAR, sway(20.0, 0.35)) - walk * CREEP;
  color = lerp(color, coarse, rim(near));
  color = lerp(color, fine, rim(far));

  const float3 shade = float3(0.25, 0.75, 1.0);
  const float3 paper = float3(1.0, 1.0, 1.0);
  color = lerp(lerp(color, shade, 0.7), paper, 0.7);
  return float4(color, 1.0);
}
