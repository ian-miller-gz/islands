var<private> v : vec2<f32>;

var<private> v_1 : vec4<f32>;

var<private> v_2 : vec4<f32>;

fn main_inner(v_3 : vec2<f32>, v_4 : vec2<f32>, v_5 : vec4<f32>) {
  v = v_4;
  v_1 = v_5;
  v_2 = vec4<f32>(v_3.x, v_3.y, 0.0f, 1.0f);
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
fn main(@location(0u) v_6 : vec2<f32>, @location(1u) v_7 : vec2<f32>, @location(2u) v_8 : vec4<f32>) -> tint_symbol_1 {
  main_inner(v_6, v_7, v_8);
  return tint_symbol_1(v, v_1, v_2);
}
