#version 120

#define Image sampler2D
#define extern uniform
#define Texel texture2D

varying vec4 color_out;
varying vec2 tex_coord_out;

extern Image PRIVATE_SOL_TEX;
extern vec2 screen_coords;

vec4 effect(vec4 color, Image tex, vec2 tc, vec2 sc);

void main(vec4 color, Image texture, vec2 texture_coords, ) {
  gl_FragColor = effect(color_out, PRIVATE_SOL_TEX, tex_coord_out, screen_coords);
}
