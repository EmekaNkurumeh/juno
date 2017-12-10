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
Buffer *m_graphics_screen;


static void resetVideoMode(lua_State *L) {
  /* Reset video mode */
  int flags = (fullscreen ? SDL_FULLSCREEN : 0) |
              (resizable  ? SDL_RESIZABLE : 0)  |
              (borderless ? SDL_NOFRAME : 0);
  const SDL_VideoInfo* info = SDL_GetVideoInfo();
  if (!info) {
    luaL_error(L," video query failed: %s", SDL_GetError());
  }
  int bpp = info->vfmt->BitsPerPixel;
  if (SDL_SetVideoMode(screenWidth, screenHeight, bpp, flags) == 0) {
    luaL_error(L, "could not set video mode: %s", SDL_GetError());
  }
  /* Reset screen buffer */
  if (m_graphics_screen) {
    sr_Buffer *b = m_graphics_screen->buffer;
    b->pixels = (void*) SDL_GetVideoSurface()->pixels;
    b->w = screenWidth;
    b->h = screenHeight;
    sr_setClip(b, sr_rect(0, 0, b->w, b->h));
  }
}


static int l_graphics_init(lua_State *L) {
  screenWidth = luaL_checkint(L, 1);
  screenHeight = luaL_checkint(L, 2);
  const char *title = luaL_optstring(L, 3, "sol");
  fullscreen = luax_optboolean(L, 4, 0);
  resizable = luax_optboolean(L, 5, 0);
  borderless = luax_optboolean(L, 6, 0);
  if (inited) {
    luaL_error(L, "graphics are already inited");
  }
  if (SDL_Init(SDL_INIT_VIDEO) != 0) {
    luaL_error(L, "could not init video");
  }
  #ifdef SOL_RELEASE
  /* Fix output rerouting */
  freopen( "CON", "w", stdout );
  freopen( "CON", "w", stderr );
  #endif
  /* Init SDL video */
  resetVideoMode(L);
  /* Required to get the associated character when a key is pressed. This has
   * to be enabled *after* SDL video is set up */
  SDL_EnableUNICODE(1);
  /* Init window title */
  SDL_WM_SetCaption(title, title);
  /* Create, store in registry and return main screen buffer */
  m_graphics_screen = buffer_new(L);
  m_graphics_screen->buffer = sr_newBufferShared(
    SDL_GetVideoSurface()->pixels, screenWidth, screenHeight);
  lua_pushvalue(L, -1);
  screenRef = lua_ref(L, LUA_REGISTRYINDEX);
  /* Set state */
  inited = 1;
  return 1;
}


static int l_graphics_setSize(lua_State *L) {
  int width = luaL_optnumber(L, 1, screenWidth);
  int height = luaL_optnumber(L, 2, screenHeight);
  int isEvent = luax_optboolean(L, 3, 0);
  if (!isEvent) {
    resetVideoMode(L);
  } else {
    /* Reset screen buffer */
    if (m_graphics_screen) {
      sr_Buffer *b = m_graphics_screen->buffer;
      b->pixels = (void*)SDL_GetVideoSurface()->pixels;
      b->w = width;
      b->h = height;
      sr_setClip(b, sr_rect(0, 0, b->w, b->h));
    }
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
