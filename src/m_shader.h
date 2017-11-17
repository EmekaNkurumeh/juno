/**
 * Copyright (c) 2017 emekoi
 *
 * This library is free software; you can redistribute it and/or modify it
 * under the terms of the MIT license. See LICENSE for details.
 */


#ifndef M_SHADER_H
#define M_SHADER_H

#include "luax.h"
#include "lib/glew/glew.h"

#define SHADER_CLASS_NAME "Shader"

#define shader_use(self) glUseProgram(self->program)

// #define UniformS(size, type) glUniform##size##type
// #define UniformV(size, type)
// https://bitbucket.org/rude/love/src/37e1ced87caf25db7b2a5a8f502af7f3b6eacbd2/src/modules/graphics/opengl/Shader.cpp?at=default&fileviewer=file-view-default
typedef struct {
  GLuint vertex, fragment;
  GLuint program;
} Shader;

// enum {
//   SCALAR,
//   VECTOR
// };

int l_shader_fromString(lua_State *L);
int l_shader_fromFile(lua_State *L);
int l_shader_setAttribute(lua_State *L);
// void shader_setUniform(Shader *self, const char *name, void *data);

// Shader *shader_fromString(const char *vertex, const char *v, const char *fragment, const char *f);
// Shader *shader_fromFile(const char *vertex, const char *fragment);
// void shader_setAttribute(Shader *self, const char *name, int size, int type, int norm, int s, void *p);
// void shader_setUniform(Shader *self, const char *name, void *data);
// void shader_destroy(Shader *self);

#endif
