/**
 * Copyright (c) 2017 emekoi
 *
 * This library is free software; you can redistribute it and/or modify it
 * under the terms of the MIT license. See LICENSE for details.
 */


#include <stdlib.h>
#include <string.h>
#include "m_shader.h"
#include "m_buffer.h"
#include "luax.h"
#include "util.h"
#include "fs.h"

#define CLASS_NAME SHADER_CLASS_NAME

// #define UniformS(size, type) glUniform##size##type
// #define UniformV(size, type)
// https://bitbucket.org/rude/love/src/37e1ced87caf25db7b2a5a8f502af7f3b6eacbd2/src/modules/graphics/opengl/Shader.cpp?at=default&fileviewer=file-view-default


static Shader *shader_new(lua_State *L) {
  Shader *self = lua_newuserdata(L, sizeof(*self));
  luaL_setmetatable(L, CLASS_NAME);
  memset(self, 0, sizeof(*self));
  return self;
}


static GLuint compileShader(lua_State *L, const char *name, const GLchar *src, GLenum type) {
  /* Compile vertex shader */
  const GLuint self = glCreateShader(type);
  glShaderSource(self, 1, &src, NULL);
  glCompileShader(self);
  GLint status;
  glGetShaderiv(self, GL_COMPILE_STATUS, &status);
  char buffer[2048];
  glGetShaderInfoLog(self, 2048, NULL, buffer);
  if (status != GL_TRUE) luaL_error(L, "\n\n%s\n\n%s : %s\n", src, name, buffer);
  return self;
}


// const GLchar *default_vert =
// "#version 120\n"
// "attribute vec4 sr_TexCoord;\n"
// "attribute vec4 sr_Position;\n"
// "void main() {\n"
// "  gl_Position = sr_Position;\n"
// "  gl_TexCoord[0] = sr_TexCoord;\n"
// "}\n";

#define makeShader(name, shader) \
  self->vertex = compileShader(L, "default.vert", default_vert, GL_VERTEX_SHADER); \
  self->fragment = compileShader(L, name, shader, GL_FRAGMENT_SHADER); \
  glAttachShader(self->program, self->vertex); \
  glAttachShader(self->program, self->fragment); \
  glLinkProgram(self->program)

static int l_shader_fromString(lua_State *L) {
  const char *shader = luaL_checkstring(L, 1);
  Shader *self = shader_new(L);
  self->program = glCreateProgram();
  #include "default_vert.h"
  makeShader("fragment shader", shader);
  return 1;
}


static int l_shader_fromFile(lua_State *L) {
  const char *filename = luaL_checkstring(L, 1);
  GLchar *source = fs_read(filename, NULL);
  Shader *self = shader_new(L);
  self->program =  glCreateProgram();
  #include "default_vert.h"
  makeShader(filename, source);
  free(source);
  return 1;
}


static int l_shader_gc(lua_State *L) {
  Shader *self = luaL_checkudata(L, 1, CLASS_NAME);
  if (self) {
    printf("%s has been GC", CLASS_NAME);
    glDetachShader(self->program, self->vertex);
    glDetachShader(self->program, self->fragment);
    glDeleteShader(self->vertex);
    glDeleteShader(self->fragment);
    glDeleteProgram(self->program);
  }
  return 0;
}


void shader_setAttribute(Shader *S, const char *name, int size, int type, int norm, int s, void *p) {
  GLint attrib = glGetAttribLocation(S->program, name);
  glVertexAttribPointer(attrib, size, type, norm, s, p);
  glEnableVertexAttribArray(attrib);
}


static int l_shader_getWarnings(lua_State *L) {
  Shader *self = luaL_checkudata(L, 1, CLASS_NAME);
  char buffer[2048];
  glGetShaderInfoLog(self->program, 2048, NULL, buffer);
  lua_pushfstring(L, "%s", buffer);
  return 0;
}


#define splice(x, y) x##y

static int l_shader_uniform(lua_State *L) {
  Shader *self = luaL_checkudata(L, 1, CLASS_NAME);
  const char * name = luaL_checkstring(L, 2);
  // GLint uniform = glGetUniformLocation(self->program, name);
  // glUniform1f(uniform, time_getDelta());

  switch (lua_type(L, 3)) {
    case LUA_TNIL: case LUA_TSTRING: case LUA_TFUNCTION:
    case LUA_TTHREAD: case LUA_TLIGHTUSERDATA:
      luaL_argerror(L, 3, "unsupported uniform type"); break;
    case LUA_TNUMBER:
    case LUA_TBOOLEAN:
    case LUA_TTABLE: TRACE("SOL_DEBUG: %s\n", lua_typename(L, lua_type(L, 3))); break;
    case LUA_TUSERDATA: {
      // Buffer *buf = luaL_checkudata(L, 3, BUFFER_CLASS_NAME);
      TRACE("SOL_DEBUG: %s\n", BUFFER_CLASS_NAME); break;
    }
  }
  return 0;
}


// void shader_setUniform(Shader *S, const char *name, void *data) {
//   GLint uniform = glGetUniformLocation(self->program, name);
//   set_uniform(uniform, 0, 0, 0);
//   // glUniform3f(uniform, sin(time) * 2, cos(time) * 2, tan(time) * 2);
// }


int luaopen_shader(lua_State *L) {
  luaL_Reg reg[] = {
    { "__gc",           l_shader_gc          },
    { "fromFile",       l_shader_fromFile    },
    { "fromString",     l_shader_fromString  },
    { "getWarnings",    l_shader_getWarnings },
    { "uniform",        l_shader_uniform     },
    { NULL, NULL }
  };
  ASSERT( luaL_newmetatable(L, CLASS_NAME) );
  luaL_setfuncs(L, reg, 0);
  lua_pushvalue(L, -1);
  lua_setfield(L, -2, "__index");
  return 1;
}
