struct S {
  Matrix : mat4x4<f32>,
  Viewport : vec4<f32>,
}

@group(1u) @binding(0u) var<uniform> UniformBlock : S;

var<private> v : vec4<f32>;

var<private> v_1 : vec4<f32>;

var<private> v_2 : vec2<f32>;

var<private> v_3 : vec4<f32>;

var<private> v_4 : vec4<i32>;

fn main_inner(v_5 : vec4<f32>, v_6 : vec4<f32>, v_7 : vec4<f32>, v_8 : vec4<f32>, v_9 : vec4<f32>) {
  let v_10 = tint_load_row_major_column(&(UniformBlock.Matrix), 0u);
  let v_11 = tint_load_row_major_column(&(UniformBlock.Matrix), 1u);
  let v_12 = tint_load_row_major_column(&(UniformBlock.Matrix), 3u);
  let v_13 = UniformBlock.Viewport;
  let v_14 = v_5.zw;
  let v_15 = normalize(v_14);
  let v_16 = v_12.xy;
  let v_17 = v_5.xy;
  let v_18 = (dot(v_16, v_17) + v_12.w);
  let v_19 = dot(v_16, v_15);
  let v_20 = v_10.xy;
  let v_21 = (((v_18 * dot(v_20, v_15)) - (v_19 * (dot(v_20, v_17) + v_10.w))) * v_13.x);
  let v_22 = v_11.xy;
  let v_23 = (((v_18 * dot(v_22, v_15)) - (v_19 * (dot(v_22, v_17) + v_11.w))) * v_13.y);
  let v_24 = (v_18 * v_19);
  let v_25 = ((v_21 * v_21) + (v_23 * v_23));
  let v_26 = (v_14 * (((v_18 * v_18) * (v_24 + sqrt(v_25))) / (v_25 - (v_24 * v_24))));
  let v_27 = (v_17 + v_26);
  let v_28 = vec2<f32>((v_6.x + dot(v_26, v_7.xy)), (v_6.y + dot(v_26, v_7.zw)));
  let v_29 = (vec4<f32>(v_27.x, v_27.y, 0.0f, 1.0f) * transpose(UniformBlock.Matrix));
  let v_30 = bitcast<vec2<u32>>(v_6.zw);
  let v_31 = v_30.x;
  let v_32 = v_30.y;
  let v_33 = vec4<i32>(bitcast<i32>((v_31 & 65535u)), bitcast<i32>((v_31 >> 16u)), bitcast<i32>((v_32 & 65535u)), bitcast<i32>((v_32 >> 16u)));
  v = v_29;
  v_1 = v_9;
  v_2 = v_28;
  v_3 = v_8;
  v_4 = v_33;
}

struct tint_symbol_1 {
  @builtin(position)
  tint_symbol : vec4<f32>,
  @location(0u)
  m : vec4<f32>,
  @location(1u)
  m_1 : vec2<f32>,
  @location(2u) @interpolate(flat)
  m_2 : vec4<f32>,
  @location(3u) @interpolate(flat)
  m_3 : vec4<i32>,
}

@vertex
fn main(@location(0u) v_34 : vec4<f32>, @location(1u) v_35 : vec4<f32>, @location(2u) v_36 : vec4<f32>, @location(3u) v_37 : vec4<f32>, @location(4u) v_38 : vec4<f32>) -> tint_symbol_1 {
  main_inner(v_34, v_35, v_36, v_37, v_38);
  return tint_symbol_1(v, v_1, v_2, v_3, v_4);
}

fn tint_load_row_major_column(v_39 : ptr<uniform, mat4x4<f32>>, v_40 : u32) -> vec4<f32> {
  return vec4<f32>((*(v_39))[0u][v_40], (*(v_39))[1u][v_40], (*(v_39))[2u][v_40], (*(v_39))[3u][v_40]);
}
