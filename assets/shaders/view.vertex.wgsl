var<private> v : vec2<f32>;

var<private> v_1 : vec4<f32>;

fn main_inner(v_2 : u32) {
  var corners : array<vec2<f32>, 3u>;
  corners = array<vec2<f32>, 3u>(vec2<f32>(-1.0f), vec2<f32>(3.0f, -1.0f), vec2<f32>(-1.0f, 3.0f));
  let v_3 = corners[v_2];
  let v_4 = v_3.x;
  let v_5 = v_3.y;
  v = vec2<f32>(((v_4 + 1.0f) * 0.5f), ((1.0f - v_5) * 0.5f));
  v_1 = vec4<f32>(v_4, v_5, 0.0f, 1.0f);
}

struct tint_symbol_1 {
  @location(0u)
  m : vec2<f32>,
  @builtin(position)
  tint_symbol : vec4<f32>,
}

@vertex
fn main(@builtin(vertex_index) v_6 : u32) -> tint_symbol_1 {
  main_inner(v_6);
  return tint_symbol_1(v, v_1);
}
