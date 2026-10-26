struct Element {
  Position : vec2<f32>,
  Extent : vec2<f32>,
  Frame : vec4<f32>,
  Color : vec4<f32>,
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

var<private> v_2 : vec4<f32>;

fn main_inner(v_3 : u32) {
  var VERTEX_POSITION : array<vec2<f32>, 4u>;
  var TRIANGLE_INDICES : array<u32, 6u>;
  TRIANGLE_INDICES = array<u32, 6u>(0u, 1u, 2u, 3u, 2u, 1u);
  VERTEX_POSITION = array<vec2<f32>, 4u>(vec2<f32>(), vec2<f32>(1.0f, 0.0f), vec2<f32>(0.0f, 1.0f), vec2<f32>(1.0f));
  let v_4 = TRIANGLE_INDICES[(v_3 % 6u)];
  let v_5 = elements.tint_symbol[(v_3 / 6u)];
  let v_6 = v_5.Frame;
  let v_7 = VERTEX_POSITION[v_4];
  let v_8 = ((v_7 * v_5.Extent) + v_5.Position);
  let v_9 = (vec4<f32>(v_8.x, v_8.y, 0.0f, 1.0f) * transpose(Uniform.ViewProjectionMatrix));
  v = (v_6.xy + (v_7 * v_6.zw));
  v_1 = v_5.Color;
  v_2 = v_9;
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
fn main(@builtin(vertex_index) v_10 : u32) -> tint_symbol_2 {
  main_inner(v_10);
  return tint_symbol_2(v, v_1, v_2);
}
