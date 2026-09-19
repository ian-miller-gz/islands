struct S {
  Color : vec4<f32>,
  Factors : vec4<f32>,
}

@group(3u) @binding(0u) var<uniform> UniformBlock : S;

var<private> v : vec4<f32>;

fn main_inner(v_1 : vec3<f32>) {
  let v_2 = normalize(vec3<f32>(0.40000000596046447754f, 0.69999998807907104492f, 0.60000002384185791016f));
  let v_3 = normalize(v_1);
  let v_4 = max(0.0f, dot(v_3, v_2));
  let v_5 = exp2((1.0f + (10.0f * (1.0f - UniformBlock.Factors.y))));
  let v_6 = pow(max(0.0f, dot(v_3, normalize((v_2 + vec3<f32>(0.0f, 0.0f, 1.0f))))), v_5);
  let v_7 = UniformBlock.Color.xyz;
  let v_8 = UniformBlock.Factors.x;
  let v_9 = (((v_7 * (1.0f - v_8)) * (0.25f + (0.75f * v_4))) + (mix(vec3<f32>(0.03999999910593032837f), v_7, vec3<f32>(v_8, v_8, v_8)) * v_6));
  v = vec4<f32>(v_9.x, v_9.y, v_9.z, UniformBlock.Color.w);
}

@fragment
fn main(@location(0u) v_10 : vec3<f32>) -> @location(0u) vec4<f32> {
  main_inner(v_10);
  return v;
}
