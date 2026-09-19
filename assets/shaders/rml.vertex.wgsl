struct S {
  ViewProjectionMatrix : mat4x4<f32>,
}

@group(1u) @binding(0u) var<uniform> Camera : S;

struct S_1 {
  Translation : vec4<f32>,
}

@group(1u) @binding(1u) var<uniform> Offset : S_1;

var<private> v : vec2<f32>;

var<private> v_1 : vec4<f32>;

var<private> v_2 : vec4<f32>;

fn main_inner(v_3 : vec2<f32>, v_4 : vec2<f32>, v_5 : vec4<f32>) {
  let v_6 = (v_3 + Offset.Translation.xy);
  let v_7 = (vec4<f32>(v_6.x, v_6.y, 0.0f, 1.0f) * transpose(Camera.ViewProjectionMatrix));
  v = v_4;
  v_1 = v_5;
  v_2 = v_7;
}

struct tint_symbol_1 {
  @location(0u)
  m : vec2<f32>,
  @location(1u)
  m_1 : vec4<f32>,
  @builtin(position)
  tint_symbol : vec4<f32>,
}

@vertex
fn main(@location(0u) v_8 : vec2<f32>, @location(1u) v_9 : vec2<f32>, @location(2u) v_10 : vec4<f32>) -> tint_symbol_1 {
  main_inner(v_8, v_9, v_10);
  return tint_symbol_1(v, v_1, v_2);
}
