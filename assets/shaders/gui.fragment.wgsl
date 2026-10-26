@group(2u) @binding(0u) var Texture : texture_2d<f32>;

@group(2u) @binding(1u) var Sampler : sampler;

var<private> v : vec4<f32>;

fn main_inner(v_1 : vec2<f32>, v_2 : vec4<f32>) {
  v = (v_2 * textureSample(Texture, Sampler, v_1));
}

@fragment
fn main(@location(0u) v_3 : vec2<f32>, @location(1u) v_4 : vec4<f32>) -> @location(0u) vec4<f32> {
  main_inner(v_3, v_4);
  return v;
}
