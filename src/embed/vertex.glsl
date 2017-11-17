#version 120

#define Image sampler2D
#define extern uniform
#define Texel texture2D

attribute vec2 tex_coord_in;
attribute vec2 position_in;
attribute vec3 color_in;

varying vec3 color_out;
varying vec2 tex_coord_out;

vec4 vertex(vec4);

void main() {
  color_out = color_in;
  gl_Position = vec4(position_in, 1.0, 1.0);
  tex_coord_out = tex_coord_in;
}
