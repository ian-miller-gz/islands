@group(2u) @binding(0u) var Curves : texture_2d<f32>;

@group(2u) @binding(1u) var Bands : texture_2d<f32>;

var<private> v : vec4<f32>;

fn main_inner(v_1 : vec4<f32>, v_2 : vec2<f32>, v_3 : vec4<f32>, v_4 : vec4<i32>) {
  let v_5 = (vec2<f32>(1.0f) / fwidth(v_2));
  let v_6 = (v_4.w & 255i);
  var v_7 : vec2<i32> = v_4.zw;
  v_7.y = v_6;
  let v_8 = clamp(vec2<i32>(((v_2 * v_3.xy) + v_3.zw)), vec2<i32>(), v_7);
  let v_9 = v_4.x;
  let v_10 = v_4.y;
  let v_11 = vec2<u32>(textureLoad(Bands, vec3<i32>((v_9 + v_8.y), v_10, 0i).xy, 0i).xy);
  let v_12 = (v_9 + bitcast<i32>(v_11.y));
  let v_13 = (v_10 + (v_12 >> bitcast<u32>(12i)));
  var v_14 : f32;
  var v_15 : f32;
  {
    var v_16 : f32;
    v_16 = 0.0f;
    var v_17 : f32;
    v_17 = 0.0f;
    var v_18 : i32;
    v_18 = 0i;
    loop {
      var v_19 : f32;
      var v_20 : f32;
      let v_21 = v_18;
      let v_22 = v_17;
      let v_23 = v_16;
      if ((v_21 < bitcast<i32>(v_11.x))) {
        let v_24 = vec2<i32>(textureLoad(Bands, vec3<i32>(((v_12 & 4095i) + v_21), v_13, 0i).xy, 0i).xy);
        let v_25 = v_24.x;
        let v_26 = v_24.y;
        let v_27 = v_2.x;
        let v_28 = v_2.y;
        let v_29 = (textureLoad(Curves, vec3<i32>(v_25, v_26, 0i).xy, 0i) - vec4<f32>(v_27, v_28, v_27, v_28));
        let v_30 = (textureLoad(Curves, vec3<i32>((v_25 + 1i), v_26, 0i).xy, 0i).xy - v_2);
        let v_31 = v_29.x;
        let v_32 = v_5.x;
        if (((max(max(v_31, v_29.z), v_30.x) * v_32) < -0.5f)) {
          v_14 = v_22;
          v_15 = v_23;
          break;
        }
        let v_33 = v_29.y;
        let v_34 = (bitcast<u32>(v_33) >> 31u);
        let v_35 = (bitcast<u32>(v_29.w) >> 30u);
        let v_36 = ((11892u >> ((((bitcast<u32>(v_30.y) >> 29u) & 4u) | (((v_35 & 2u) | (v_34 & 4294967293u)) & 4294967291u)) & 31u)) & 257u);
        var v_37 : f32;
        var v_38 : f32;
        if ((v_36 != 0u)) {
          let v_39 = v_29.xy;
          let v_40 = v_29.zw;
          let v_41 = ((v_39 - (v_40 * 2.0f)) + v_30);
          let v_42 = (v_39 - v_40);
          let v_43 = v_41.y;
          let v_44 = (1.0f / v_43);
          let v_45 = v_42.y;
          let v_46 = sqrt(max(((v_45 * v_45) - (v_43 * v_33)), 0.0f));
          var v_47 : f32;
          var v_48 : f32;
          if ((abs(v_43) < 0.0000152587890625f)) {
            let v_49 = (v_33 * (0.5f / v_45));
            v_47 = v_49;
            v_48 = v_49;
          } else {
            v_47 = ((v_45 + v_46) * v_44);
            v_48 = ((v_45 - v_46) * v_44);
          }
          let v_50 = v_48;
          let v_51 = v_47;
          let v_52 = v_41.x;
          let v_53 = (v_42.x * 2.0f);
          let v_54 = (vec2<f32>(((((v_52 * v_50) - v_53) * v_50) + v_31), ((((v_52 * v_51) - v_53) * v_51) + v_31)) * v_32);
          var v_55 : f32;
          var v_56 : f32;
          if (((v_36 & 1u) != 0u)) {
            let v_57 = v_54.x;
            let v_58 = (v_22 + clamp((v_57 + 0.5f), 0.0f, 1.0f));
            v_55 = max(v_23, clamp((1.0f - (abs(v_57) * 2.0f)), 0.0f, 1.0f));
            v_56 = v_58;
          } else {
            v_55 = v_23;
            v_56 = v_22;
          }
          let v_59 = v_56;
          let v_60 = v_55;
          var v_61 : f32;
          var v_62 : f32;
          if ((v_36 > 1u)) {
            let v_63 = v_54.y;
            let v_64 = (v_59 - clamp((v_63 + 0.5f), 0.0f, 1.0f));
            v_61 = max(v_60, clamp((1.0f - (abs(v_63) * 2.0f)), 0.0f, 1.0f));
            v_62 = v_64;
          } else {
            v_61 = v_60;
            v_62 = v_59;
          }
          let v_65 = v_62;
          v_37 = v_61;
          v_38 = v_65;
        } else {
          v_37 = v_23;
          v_38 = v_22;
        }
        let v_66 = v_38;
        v_19 = v_37;
        v_20 = v_66;
        continue;
      } else {
        v_14 = v_22;
        v_15 = v_23;
        break;
      }

      continuing {
        let v_67 = v_20;
        v_16 = v_19;
        v_17 = v_67;
        v_18 = (v_21 + 1i);
      }
    }
  }
  let v_68 = v_15;
  let v_69 = v_14;
  let v_70 = vec2<u32>(textureLoad(Bands, vec3<i32>((((v_9 + v_6) + 1i) + v_8.x), v_10, 0i).xy, 0i).xy);
  let v_71 = (v_9 + bitcast<i32>(v_70.y));
  let v_72 = (v_10 + (v_71 >> bitcast<u32>(12i)));
  var v_73 : f32;
  var v_74 : f32;
  {
    var v_75 : f32;
    v_75 = 0.0f;
    var v_76 : f32;
    v_76 = 0.0f;
    var v_77 : i32;
    v_77 = 0i;
    loop {
      var v_78 : f32;
      var v_79 : f32;
      let v_80 = v_77;
      let v_81 = v_76;
      let v_82 = v_75;
      if ((v_80 < bitcast<i32>(v_70.x))) {
        let v_83 = vec2<i32>(textureLoad(Bands, vec3<i32>(((v_71 & 4095i) + v_80), v_72, 0i).xy, 0i).xy);
        let v_84 = v_83.x;
        let v_85 = v_83.y;
        let v_86 = v_2.x;
        let v_87 = v_2.y;
        let v_88 = (textureLoad(Curves, vec3<i32>(v_84, v_85, 0i).xy, 0i) - vec4<f32>(v_86, v_87, v_86, v_87));
        let v_89 = (textureLoad(Curves, vec3<i32>((v_84 + 1i), v_85, 0i).xy, 0i).xy - v_2);
        let v_90 = v_88.y;
        let v_91 = v_5.y;
        if (((max(max(v_90, v_88.w), v_89.y) * v_91) < -0.5f)) {
          v_73 = v_81;
          v_74 = v_82;
          break;
        }
        let v_92 = v_88.x;
        let v_93 = (bitcast<u32>(v_92) >> 31u);
        let v_94 = (bitcast<u32>(v_88.z) >> 30u);
        let v_95 = ((11892u >> ((((bitcast<u32>(v_89.x) >> 29u) & 4u) | (((v_94 & 2u) | (v_93 & 4294967293u)) & 4294967291u)) & 31u)) & 257u);
        var v_96 : f32;
        var v_97 : f32;
        if ((v_95 != 0u)) {
          let v_98 = v_88.xy;
          let v_99 = v_88.zw;
          let v_100 = ((v_98 - (v_99 * 2.0f)) + v_89);
          let v_101 = (v_98 - v_99);
          let v_102 = v_100.x;
          let v_103 = (1.0f / v_102);
          let v_104 = v_101.x;
          let v_105 = sqrt(max(((v_104 * v_104) - (v_102 * v_92)), 0.0f));
          var v_106 : f32;
          var v_107 : f32;
          if ((abs(v_102) < 0.0000152587890625f)) {
            let v_108 = (v_92 * (0.5f / v_104));
            v_106 = v_108;
            v_107 = v_108;
          } else {
            v_106 = ((v_104 + v_105) * v_103);
            v_107 = ((v_104 - v_105) * v_103);
          }
          let v_109 = v_107;
          let v_110 = v_106;
          let v_111 = v_100.y;
          let v_112 = (v_101.y * 2.0f);
          let v_113 = (vec2<f32>(((((v_111 * v_109) - v_112) * v_109) + v_90), ((((v_111 * v_110) - v_112) * v_110) + v_90)) * v_91);
          var v_114 : f32;
          var v_115 : f32;
          if (((v_95 & 1u) != 0u)) {
            let v_116 = v_113.x;
            let v_117 = (v_81 - clamp((v_116 + 0.5f), 0.0f, 1.0f));
            v_114 = max(v_82, clamp((1.0f - (abs(v_116) * 2.0f)), 0.0f, 1.0f));
            v_115 = v_117;
          } else {
            v_114 = v_82;
            v_115 = v_81;
          }
          let v_118 = v_115;
          let v_119 = v_114;
          var v_120 : f32;
          var v_121 : f32;
          if ((v_95 > 1u)) {
            let v_122 = v_113.y;
            let v_123 = (v_118 + clamp((v_122 + 0.5f), 0.0f, 1.0f));
            v_120 = max(v_119, clamp((1.0f - (abs(v_122) * 2.0f)), 0.0f, 1.0f));
            v_121 = v_123;
          } else {
            v_120 = v_119;
            v_121 = v_118;
          }
          let v_124 = v_121;
          v_96 = v_120;
          v_97 = v_124;
        } else {
          v_96 = v_82;
          v_97 = v_81;
        }
        let v_125 = v_97;
        v_78 = v_96;
        v_79 = v_125;
        continue;
      } else {
        v_73 = v_81;
        v_74 = v_82;
        break;
      }

      continuing {
        let v_126 = v_79;
        v_75 = v_78;
        v_76 = v_126;
        v_77 = (v_80 + 1i);
      }
    }
  }
  let v_127 = v_74;
  let v_128 = v_73;
  v = (v_1 * clamp(max((abs(((v_69 * v_68) + (v_128 * v_127))) / max((v_68 + v_127), 0.0000152587890625f)), min(abs(v_69), abs(v_128))), 0.0f, 1.0f));
}

@fragment
fn main(@location(0u) v_129 : vec4<f32>, @location(1u) v_130 : vec2<f32>, @location(2u) @interpolate(flat) v_131 : vec4<f32>, @location(3u) @interpolate(flat) v_132 : vec4<i32>) -> @location(0u) vec4<f32> {
  main_inner(v_129, v_130, v_131, v_132);
  return v;
}
