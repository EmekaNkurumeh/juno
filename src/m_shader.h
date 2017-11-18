/**
 * Copyright (c) 2017 emekoi
 *
 * This library is free software; you can redistribute it and/or modify it
 * under the terms of the MIT license. See LICENSE for details.
 */


#ifndef M_SHADER_H
#define M_SHADER_H

#include "lib/glew/glew.h"

#define SHADER_CLASS_NAME "Shader"

typedef struct {
  GLuint vertex, fragment;
  GLuint program;
} Shader;

void shader_setAttribute(Shader *S, const char *name, int size, int type, int norm, int s, void *p);

#endif
