/**
 * Copyright (c) 2015 rxi
 *
 * This library is free software; you can redistribute it and/or modify it
 * under the terms of the MIT license. See LICENSE for details.
 */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <float.h>
#include <SDL/SDL.h>
#include "lib/sera/sera.h"
#include "util.h"
#include "luax.h"
#include "m_buffer.h"

double m_graphics_maxFps = 60.;

static int inited = 0;
static int screenWidth = 0;
static int screenHeight = 0;
static int screenRef = 0;
static int fullscreen = 0;
static int resizable = 0;
static int borderless = 0;
Buffer *screen;


static void resetVideoMode(lua_State *L) {
  /* Reset video mode */
  int flags = (fullscreen ? SDL_FULLSCREEN : 0) |
              (resizable  ? SDL_RESIZABLE : 0)  |
              (borderless ? SDL_NOFRAME : 0) | SDL_OPENGL;
  
  const SDL_VideoInfo* info = SDL_GetVideoInfo( );

  if(!info) luaL_error(L," video query failed");
  
  int bpp = info->vfmt->BitsPerPixel;
  
  if (SDL_SetVideoMode(screenWidth, screenHeight, bpp, flags) == NULL) {
    luaL_error(L, "could not set video mode");
  }
  /* Reset screen buffer */
  if (screen) {
    sr_Buffer *b = screen->buffer;
    b->pixels = realloc(b->pixels, b->w * b->h * sizeof(*b->pixels));
    b->w = screenWidth;
    b->h = screenHeight;
    sr_setClip(b, sr_rect(0, 0, b->w, b->h));
  }
}


static int l_graphics_init(lua_State *L) {
  screenWidth = luaL_checkint(L, 1);
  screenHeight = luaL_checkint(L, 2);
  const char *title = luaL_optstring(L, 3, "Sol");
  fullscreen = luax_optboolean(L, 4, 0);
  resizable = luax_optboolean(L, 5, 0);
  borderless = luax_optboolean(L, 6, 0);
  if (inited) {
    luaL_error(L, "graphics are already inited");
  }
  if (SDL_Init(SDL_INIT_VIDEO) != 0) {
    luaL_error(L, "could not init video");
  }
  
  /* Setup OpenGL */
  SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
  
  /* Init SDL video */
  resetVideoMode(L);
  
  /* Init GLEW */
  glewExperimental = GL_TRUE;
  glewInit(); GLuint vertexbuf;
  glGenBuffers(1, &vertexbuf);
  if (!vertexbuf) CERROR("failed to init GLEW");

  /* OpenGL config */
  glDisable(GL_DEPTH_TEST);
  glDisable(GL_CULL_FACE);
  
  /* Required to get the associated character when a key is pressed. This has
   * to be enabled *after* SDL video is set up */
  SDL_EnableUNICODE(1);
  /* Init window title */
  SDL_WM_SetCaption(title, title);
  /* Create, store in registry and return main screen buffer */
  screen = buffer_new(L);
  screen->buffer = sr_newBuffer(screenWidth, screenHeight);
  lua_pushvalue(L, -1);
  screenRef = lua_ref(L, LUA_REGISTRYINDEX);
  /* Set state */
  inited = 1;
  return 1;
}


static int l_graphics_setSize(lua_State *L) {
  int width = luaL_optnumber(L, 1, screenWidth);
  int height = luaL_optnumber(L, 2, screenHeight);
  /* Reset video mode and set new screen size*/
  int flags = (fullscreen ? SDL_FULLSCREEN : 0) |
              (resizable  ? SDL_RESIZABLE : 0)  |
              (borderless ? SDL_NOFRAME : 0);
  if (SDL_SetVideoMode(width, height, 32, flags) == NULL) {
    luaL_error(L, "could not set resize screen");
  }
  /* Reset screen buffer */
  if (screen) {
    sr_Buffer *b = screen->buffer;
    b->pixels = realloc(b->pixels, b->w * b->h * sizeof(*b->pixels));
    b->w = width;
    b->h = height;
    sr_setClip(b, sr_rect(0, 0, b->w, b->h));
  }
  return 0;
}

static int l_graphics_setFullscreen(lua_State *L) {
  fullscreen = luax_optboolean(L, 1, 0);
  resetVideoMode(L);
  return 0;
}

static int l_graphics_getFullscreen(lua_State *L) {
  lua_pushboolean(L, fullscreen);
  return 1;
}

static int l_graphics_setMaxFps(lua_State *L) {
  m_graphics_maxFps = luaL_optnumber(L, 1, 60);
  return 0;
}

static int l_graphics_getMaxFps(lua_State *L) {
  lua_pushnumber(L, m_graphics_maxFps);
  return 1;
}


int luaopen_graphics(lua_State *L) {
  luaL_Reg reg[] = {
    { "init",           l_graphics_init           },
    { "setSize",        l_graphics_setSize        },
    { "setFullscreen",  l_graphics_setFullscreen  },
    { "getFullscreen",  l_graphics_getFullscreen  },
    { "setMaxFps",      l_graphics_setMaxFps      },
    { "getMaxFps",      l_graphics_getMaxFps      },
    { NULL, NULL }
  };
  luaL_newlib(L, reg);
  return 1;
}
