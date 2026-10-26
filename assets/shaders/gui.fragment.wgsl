@group(2u) @binding(0u) var Texture : texture_2d<f32>;

@group(2u) @binding(1u) var Sampler : sampler;

var<private> v : vec4<f32>;

fn main_inner(v_1 : vec2<f32>, v_2 : vec4<f32>, v_3 : vec3<f32>, v_4 : vec2<f32>) {
  let v_5 = (v_2 * textureSample(Texture, Sampler, v_1));
  let v_6 = v_3.z;
  var v_7 : vec4<f32>;
  if ((v_6 > 0.0f)) {
    var v_8 : vec4<f32> = v_5;
    v_8.w = (v_5.w * clamp(((min((v_3.y - abs(v_3.x)), min(v_4.x, v_4.y)) / v_6) + 0.5f), 0.0f, 1.0f));
    v_7 = v_8;
  } else {
    v_7 = v_5;
  }
  v = v_7;
}

@fragment
fn main(@location(0u) v_9 : vec2<f32>, @location(1u) v_10 : vec4<f32>, @location(2u) v_11 : vec3<f32>, @location(3u) v_12 : vec2<f32>) -> @location(0u) vec4<f32> {
  main_inner(v_9, v_10, v_11, v_12);
  return v;
}
