struct Sprite {
  Position : vec3<f32>,
  Rotation : f32,
  Scale : vec2<f32>,
  Padding : vec2<f32>,
  TexU : f32,
  TexV : f32,
  TexW : f32,
  TexH : f32,
  Color : vec4<f32>,
}

struct S {
  tint_symbol : array<Sprite>,
}

@group(0u) @binding(0u) var<storage, read> sprites : S;

struct S_1 {
  ViewProjectionMatrix : mat4x4<f32>,
}

@group(1u) @binding(0u) var<uniform> Uniform : S_1;

var<private> v : vec2<f32>;

var<private> v_1 : vec4<f32>;

var<private> v_2 : vec4<f32>;

fn main_inner(v_3 : u32) {
  var VERTEX_POSITION : array<vec2<f32>, 4u>;
  var TRIANGLE_INDICES : array<u32, 6u>;
  var v_4 : array<vec2<f32>, 4u>;
  TRIANGLE_INDICES = array<u32, 6u>(0u, 1u, 2u, 3u, 2u, 1u);
  VERTEX_POSITION = array<vec2<f32>, 4u>(vec2<f32>(), vec2<f32>(1.0f, 0.0f), vec2<f32>(0.0f, 1.0f), vec2<f32>(1.0f));
  let v_5 = (v_3 / 6u);
  let v_6 = TRIANGLE_INDICES[(v_3 % 6u)];
  let v_7 = sprites.tint_symbol[v_5].Position;
  let v_8 = sprites.tint_symbol[v_5].Rotation;
  let v_9 = sprites.tint_symbol[v_5].Scale;
  let v_10 = sprites.tint_symbol[v_5].TexU;
  let v_11 = sprites.tint_symbol[v_5].TexV;
  let v_12 = sprites.tint_symbol[v_5].TexW;
  let v_13 = sprites.tint_symbol[v_5].TexH;
  let v_14 = sprites.tint_symbol[v_5].Color;
  let v_15 = (v_10 + v_12);
  let v_16 = (v_11 + v_13);
  v_4 = array<vec2<f32>, 4u>(vec2<f32>(v_10, v_11), vec2<f32>(v_15, v_11), vec2<f32>(v_10, v_16), vec2<f32>(v_15, v_16));
  let v_17 = cos(v_8);
  let v_18 = sin(v_8);
  let v_19 = ((mat2x2<f32>(vec2<f32>(v_17, v_18), vec2<f32>(-(v_18), v_17)) * (VERTEX_POSITION[v_6] * v_9)) + v_7.xy);
  let v_20 = (vec4<f32>(v_19.x, v_19.y, v_7.z, 1.0f) * transpose(Uniform.ViewProjectionMatrix));
  v = v_4[v_6];
  v_1 = v_14;
  v_2 = v_20;
}

struct tint_symbol_2 {
  @location(0u)
  m : vec2<f32>,
  @location(1u)
  m_1 : vec4<f32>,
  @builtin(position)
  tint_symbol_1 : vec4<f32>,
}

@vertex
fn main(@builtin(vertex_index) v_21 : u32) -> tint_symbol_2 {
  main_inner(v_21);
  return tint_symbol_2(v, v_1, v_2);
}
