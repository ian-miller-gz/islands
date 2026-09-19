struct S {
  ViewProjectionMatrix : mat4x4<f32>,
}

@group(1u) @binding(0u) var<uniform> UniformBlock : S;

struct S_1 {
  RotationMatrix : mat4x4<f32>,
}

@group(1u) @binding(1u) var<uniform> RotationBlock : S_1;

var<private> v : vec3<f32>;

var<private> v_1 : vec2<f32>;

var<private> v_2 : vec4<f32>;

fn main_inner(v_3 : vec3<f32>, v_4 : vec3<f32>, v_5 : vec2<f32>) {
  let v_6 = (vec4<f32>(v_3.x, v_3.y, v_3.z, 1.0f) * transpose(UniformBlock.ViewProjectionMatrix));
  let v_7 = transpose(RotationBlock.RotationMatrix);
  v = normalize((v_4 * mat3x3<f32>(v_7[0u].xyz, v_7[1u].xyz, v_7[2u].xyz)));
  v_1 = v_5;
  v_2 = v_6;
}

struct tint_symbol_1 {
  @location(0u)
  m : vec3<f32>,
  @location(1u)
  m_1 : vec2<f32>,
  @builtin(position)
  tint_symbol : vec4<f32>,
}

@vertex
fn main(@location(0u) v_8 : vec3<f32>, @location(1u) v_9 : vec3<f32>, @location(2u) v_10 : vec2<f32>) -> tint_symbol_1 {
  main_inner(v_8, v_9, v_10);
  return tint_symbol_1(v, v_1, v_2);
}
