/**
 * Copyright (c) 2017 emekoi
 *
 * This library is free software; you can redistribute it and/or modify it
 * under the terms of the MIT license. See LICENSE for details.
 */


#include <stdlib.h>
#include <string.h>
#include "m_shader.h"
#include "util.h"
#include "fs.h"

#define CLASS_NAME SHADER_CLASS_NAME

Shader *m_shader_current;

Shader *shader_new(lua_State *L) {
  Shader *self = lua_newuserdata(L, sizeof(*self));
  luaL_setmetatable(L, CLASS_NAME);
  memset(self, 0, sizeof(*self));
  return self;
}


static GLuint compileShader(lua_State *L, const char *name, const GLchar *src, GLenum type) {
  /* Compile vertex shader */
  const GLuint self = glCreateShader(type);
  glShaderSource(self, 1, &src, NULL);
  glCompileShader(self); GLint status;
  glGetShaderiv(self, GL_COMPILE_STATUS, &status);
  char buffer[1024];
  glGetShaderInfoLog(self, 1024, NULL, buffer);
  if (status != GL_TRUE) luaL_error(L, "%s : %s\n", name, buffer);
  return self;
}


// Shader *shader_fromString(const char *vertex, const char *v, const char *fragment, const char *f) {
Shader *shader_fromString(lua_State *L) {
  Shader *self = shader_new(L); GLuint program;
  // self->vertex = compileShader(v, vertex, GL_VERTEX_SHADER);
  // self->fragment = compileShader(f, fragment, GL_FRAGMENT_SHADER);
  // program =  glCreateProgram();
  // glAttachShader(program, self->vertex);
  // glAttachShader(program, self->fragment);
  // glLinkProgram(program);
  // self->program = program;
  return self;
}


// Shader *shader_fromFile(const char *vertex, const char *fragment) {
Shader *shader_fromFile(lua_State *L) {
  const char *vertex = luaL_checkstring(L, 1);
  const char *fragment = luaL_checkstring(L, 1);
  Shader *self = shader_new(L);
  // GLchar *v_src = fs_read(vertex, NULL);
  // GLchar *f_src = fs_read(fragment, NULL);
  // Shader *self = shader_fromString(v_src, vertex, f_src, fragment);
  // free(v_src); free(f_src);
  return self;
}


static int l_shader_gc(lua_State *L) {
  Shader *self = luaL_checkudata(L, 1, CLASS_NAME);
  if (self) {
    glDetachShader(self->program, self->vertex);
    glDetachShader(self->program, self->fragment);
    glDeleteShader(self->vertex);
    glDeleteShader(self->fragment);
    glDeleteProgram(self->program);
  }
  return 0;
}


void shader_setAttribute(Shader *self, const char *name, int size, int type, int norm, int s, void *p) {
  GLint attrib = glGetAttribLocation(self->program, name);
  glVertexAttribPointer(attrib, size, type, norm, s, p);
  glEnableVertexAttribArray(attrib);
}


// static void set_uniform(GLuint location, int size, int vec, int type) {
//   if (vec) {

//   } else {

//   }
// }


// void shader_setUniform(Shader *S, const char *name, void *data) {
//   GLint uniform = glGetUniformLocation(self->program, name);
//   set_uniform(uniform, 0, 0, 0);
//   // glUniform3f(uniform, sin(time) * 2, cos(time) * 2, tan(time) * 2);
// }


int luaopen_shader(lua_State *L) {
  luaL_Reg reg[] = {
    { "__gc",           l_shader_gc             },
    { "fromFile",       l_shader_fromFile       },
    { "fromString",     l_shader_fromString     },
    // { "set",            l_shader_setAttribute   },
    // { "uniform",           l_shader_sendUniform    },
    { NULL, NULL }
  };
  ASSERT( luaL_newmetatable(L, CLASS_NAME) );
  luaL_setfuncs(L, reg, 0);
  lua_pushvalue(L, -1);
  lua_setfield(L, -2, "__index");
  return 1;
}
