@group(2u) @binding(0u) var Texture : texture_2d<f32>;

@group(2u) @binding(1u) var Sampler : sampler;

struct Node {
  Kind : f32,
  One : f32,
  Two : f32,
  Amount : f32,
  Value : vec4<f32>,
}

struct S {
  Count : f32,
  Surface : f32,
  Reserved : vec2<f32>,
  Nodes : array<Node, 16u>,
}

@group(3u) @binding(0u) var<uniform> Graph : S;

var<private> v : vec4<f32>;

fn main_inner(v_1 : vec3<f32>, v_2 : vec2<f32>) {
  var v_3 : array<vec4<f32>, 16u>;
  {
    var v_4 : i32;
    v_4 = 0i;
    loop {
      let v_5 = v_4;
      if ((v_5 < 16i)) {
        continue;
      } else {
        break;
      }

      continuing {
        v_3[v_5] = vec4<f32>(0.0f, 0.0f, 0.0f, 1.0f);
        v_4 = (v_5 + 1i);
      }
    }
  }
  let v_6 = min(i32(Graph.Count), 16i);
  {
    var v_7 : i32;
    v_7 = 0i;
    loop {
      let v_8 = v_7;
      if ((v_8 < v_6)) {
        let v_9 = Graph.Nodes[v_8];
        let v_10 = v_9.Kind;
        let v_11 = v_9.Amount;
        let v_12 = v_9.Value;
        let v_13 = v_3[i32(v_9.One)];
        let v_14 = v_3[i32(v_9.Two)];
        var v_15 : vec4<f32>;
        if ((v_10 == 1.0f)) {
          v_15 = (textureSample(Texture, Sampler, v_2) * v_12);
        } else {
          var v_16 : vec4<f32>;
          if ((v_10 == 2.0f)) {
            v_16 = mix(v_13, v_14, vec4<f32>(v_11, v_11, v_11, v_11));
          } else {
            var v_17 : vec4<f32>;
            if ((v_10 == 3.0f)) {
              let v_18 = (v_2 * v_11);
              let v_19 = floor(v_18);
              let v_20 = fract(v_18);
              let v_21 = ((v_20 * v_20) * (vec2<f32>(3.0f) - (v_20 * 2.0f)));
              let v_22 = fract((sin(dot(v_19, vec2<f32>(127.09999847412109375f, 311.70001220703125f))) * 43758.546875f));
              let v_23 = fract((sin(dot((v_19 + vec2<f32>(1.0f, 0.0f)), vec2<f32>(127.09999847412109375f, 311.70001220703125f))) * 43758.546875f));
              let v_24 = fract((sin(dot((v_19 + vec2<f32>(0.0f, 1.0f)), vec2<f32>(127.09999847412109375f, 311.70001220703125f))) * 43758.546875f));
              let v_25 = fract((sin(dot((v_19 + vec2<f32>(1.0f)), vec2<f32>(127.09999847412109375f, 311.70001220703125f))) * 43758.546875f));
              let v_26 = v_21.x;
              let v_27 = mix(mix(v_22, v_23, v_26), mix(v_24, v_25, v_26), v_21.y);
              v_17 = vec4<f32>(v_27, v_27, v_27, 1.0f);
            } else {
              var v_28 : vec4<f32>;
              if ((v_10 == 4.0f)) {
                let v_29 = clamp((v_2.y + v_11), 0.0f, 1.0f);
                v_28 = mix(v_13, v_14, vec4<f32>(v_29, v_29, v_29, v_29));
              } else {
                var v_30 : vec4<f32>;
                if ((v_10 == 5.0f)) {
                  let v_31 = (v_13.xyz * clamp((max(dot(normalize(v_1), -(normalize(v_12.xyz))), 0.0f) + v_11), 0.0f, 1.0f));
                  v_30 = vec4<f32>(v_31.x, v_31.y, v_31.z, v_13.w);
                } else {
                  v_30 = v_12;
                }
                v_28 = v_30;
              }
              v_17 = v_28;
            }
            v_16 = v_17;
          }
          v_15 = v_16;
        }
        v_3[v_8] = v_15;
        continue;
      } else {
        break;
      }

      continuing {
        v_7 = (v_8 + 1i);
      }
    }
  }
  v = v_3[i32(Graph.Surface)];
}

@fragment
fn main(@location(0u) v_32 : vec3<f32>, @location(1u) v_33 : vec2<f32>) -> @location(0u) vec4<f32> {
  main_inner(v_32, v_33);
  return v;
}
