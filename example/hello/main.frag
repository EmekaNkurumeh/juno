#version 120

uniform sampler2D tex;
uniform float elapsed;

vec2 radialDistortion(vec2 coord, float dist) {
  vec2 cc = coord - 0.5;
  float elapsed = 1;
  dist = dot(cc, cc) * dist + cos(elapsed * .3) * .01;
  return (coord + cc * (1.0 + dist) * dist);
}


vec4 effect(sampler2D tex, vec2 tc) {
  vec2 tcr = radialDistortion(tc, .24)  + vec2(.001, 0);
  vec2 tcg = radialDistortion(tc, .20);
  vec2 tcb = radialDistortion(tc, .18) - vec2(.001, 0);
  vec4 res = vec4(texture2D(tex, tcr).r, texture2D(tex, tcg).g, texture2D(tex, tcb).b, 1)
    - cos(tcg.y * 128. * 3.142 * 2) * .03
    - sin(tcg.x * 128. * 3.142 * 2) * .03;
  return res * texture2D(tex, tcg).a;
}


void main() {
  float x = gl_TexCoord[0].x;
  float y = gl_TexCoord[0].y;
  vec2 tc = vec2(x + sin(y * 30 + elapsed) * 10 / 1000, y);
  gl_FragColor = effect(tex, tc) + vec4(-.2, -.1, .1, 0);
}
