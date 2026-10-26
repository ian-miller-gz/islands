struct Element {
  Position : vec2<f32>,
  Extent : vec2<f32>,
  Frame : vec4<f32>,
  Color : vec4<f32>,
  Axis : vec2<f32>,
  Fringe : f32,
  Caps : f32,
}

struct S {
  tint_symbol : array<Element>,
}

@group(0u) @binding(0u) var<storage, read> elements : S;

struct S_1 {
  ViewProjectionMatrix : mat4x4<f32>,
}

@group(1u) @binding(0u) var<uniform> Uniform : S_1;

var<private> v : vec2<f32>;

var<private> v_1 : vec4<f32>;

var<private> v_2 : vec3<f32>;

var<private> v_3 : vec2<f32>;

var<private> v_4 : vec4<f32>;

fn main_inner(v_5 : u32) {
  var VERTEX_POSITION : array<vec2<f32>, 4u>;
  var TRIANGLE_INDICES : array<u32, 6u>;
  TRIANGLE_INDICES = array<u32, 6u>(0u, 1u, 2u, 3u, 2u, 1u);
  VERTEX_POSITION = array<vec2<f32>, 4u>(vec2<f32>(), vec2<f32>(1.0f, 0.0f), vec2<f32>(0.0f, 1.0f), vec2<f32>(1.0f));
  let v_6 = TRIANGLE_INDICES[(v_5 % 6u)];
  let v_7 = elements.tint_symbol[(v_5 / 6u)];
  let v_8 = v_7.Extent;
  let v_9 = v_7.Frame;
  let v_10 = v_7.Axis;
  let v_11 = v_7.Fringe;
  let v_12 = VERTEX_POSITION[v_6];
  let v_13 = (v_11 * 0.5f);
  let v_14 = u32(v_7.Caps);
  let v_15 = select(0.0f, v_13, ((v_14 & 1u) != 0u));
  let v_16 = select(0.0f, v_13, ((v_14 & 2u) != 0u));
  let v_17 = v_8.x;
  let v_18 = ((v_12.x * ((v_17 + v_15) + v_16)) - v_15);
  let v_19 = v_8.y;
  let v_20 = ((v_12.y * (v_19 + v_11)) - v_13);
  let v_21 = ((v_7.Position + (v_10 * v_18)) + (vec2<f32>(-(v_10.y), v_10.x) * v_20));
  let v_22 = (vec4<f32>(v_21.x, v_21.y, 0.0f, 1.0f) * transpose(Uniform.ViewProjectionMatrix));
  let v_23 = (v_19 * 0.5f);
  let v_24 = select(65536.0f, v_18, (v_15 > 0.0f));
  var v_25 : f32;
  if ((v_16 > 0.0f)) {
    v_25 = (v_17 - v_18);
  } else {
    v_25 = 65536.0f;
  }
  let v_26 = vec2<f32>(v_24, v_25);
  v = (v_9.xy + (v_12 * v_9.zw));
  v_1 = v_7.Color;
  v_2 = vec3<f32>((v_20 - v_23), v_23, v_11);
  v_3 = v_26;
  v_4 = v_22;
}

struct tint_symbol_2 {
  @location(0u)
  m : vec2<f32>,
  @location(1u)
  m_1 : vec4<f32>,
  @location(2u)
  m_2 : vec3<f32>,
  @location(3u)
  m_3 : vec2<f32>,
  @builtin(position)
  tint_symbol_1 : vec4<f32>,
}

@vertex
fn main(@builtin(vertex_index) v_27 : u32) -> tint_symbol_2 {
  main_inner(v_27);
  return tint_symbol_2(v, v_1, v_2, v_3, v_4);
}
