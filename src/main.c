/**
 * Copyright (c) 2015 rxi
 *
 * This library is free software; you can redistribute it and/or modify it
 * under the terms of the MIT license. See LICENSE for details.
 */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <SDL/SDL.h>
#include "lib/glew/glew.h"
#include "lib/sera/sera.h"
#include "util.h"
#include "luax.h"
#include "m_source.h"
#include "m_buffer.h"
#include "fs.h"

extern double m_graphics_maxFps;
extern Buffer* m_graphics_screen;
GLuint m_graphics_vao, m_graphics_vbo, m_graphics_ebo, m_graphics_tex;

static lua_State *L;
static SDL_mutex *luaMutex;

static void shutdown(void) {
#ifndef __APPLE__
  SDL_UnlockMutex(luaMutex);
  SDL_Quit();
#endif
  glDeleteBuffers(1, &m_graphics_ebo);
  glDeleteBuffers(1, &m_graphics_vbo);
  glDeleteVertexArrays(1, &m_graphics_vao);
  glDeleteTextures(1, &m_graphics_tex);
}

int luaopen_sol(lua_State *L);


int main(int argc, char **argv) {
  atexit(shutdown);

  /* Init lua state mutex and pass to sources module */
  luaMutex = SDL_CreateMutex();
  ASSERT(luaMutex);
  source_setLuaMutex(luaMutex);

  /* Init lua state */
  L = luaL_newstate();
  luaL_openlibs(L);

  /* Init main module -- this also inits the submodules */
  luaL_requiref(L, "sol", luaopen_sol, 1);

  /* Push command line arguments */
  lua_getglobal(L, "sol");
  if (!lua_isnil(L, -1)) {
    lua_newtable(L);
    int i;
    for (i = 0; i < argc; i++) {
      lua_pushstring(L, argv[i]);
      lua_rawseti(L, -2, i + 1);
    }
    lua_setfield(L, -2, "_argv");
  }
  lua_pop(L, 1);

  /* Init embedded scripts -- these should be ordered in the array in the order
   * we want them loaded; init.lua should always be last since it depends on
   * all the other modules */
  #include "buffer_lua.h"
  #include "time_lua.h"
  #include "graphics_lua.h"
  #include "keyboard_lua.h"
  #include "mouse_lua.h"
  #include "debug_lua.h"
  #include "system_lua.h"
  #include "init_lua.h"
  struct {
    const char *name, *data; int size;
  } items[] = {
    { "buffer.lua",     buffer_lua,     sizeof(buffer_lua)    },
    { "time.lua",       time_lua,       sizeof(time_lua)      },
    { "graphics.lua",   graphics_lua,   sizeof(graphics_lua)  },
    { "keyboard.lua",   keyboard_lua,   sizeof(keyboard_lua)  },
    { "mouse.lua",      mouse_lua,      sizeof(mouse_lua)     },
    { "debug.lua",      debug_lua,      sizeof(debug_lua)     },
    { "system.lua",     system_lua,     sizeof(system_lua)    },
    { "init.lua",       init_lua,       sizeof(init_lua)      },
    { NULL, NULL, 0 }
  };
  ASSERT(SDL_LockMutex(luaMutex) == 0);
  int i;
  for (i = 0; items[i].name; i++) {
    int err = luaL_loadbuffer(L, items[i].data, items[i].size, items[i].name);
    if (err || lua_pcall(L, 0, 0, 0) != 0) {
      const char *str = lua_tostring(L, -1);
      fprintf(stderr, "error: %s\n", str);
      abort();
    }
  }
  ASSERT(SDL_UnlockMutex(luaMutex) == 0);

  // Create Vertex Array Object
  glGenVertexArrays(1, &m_graphics_vao);
  glBindVertexArray(m_graphics_vao);

  // Create a Vertex Buffer Object and copy the vertex data to it
  glGenBuffers(1, &m_graphics_vbo);

  float vertices[] = {
  //  Position                Texcoords
  -1.0f,  1.0f, 1.0f, 1.0f,   0.0f, 0.0f, 1.0f, 1.0f, // Top-left
   1.0f,  1.0f, 1.0f, 1.0f,   1.0f, 0.0f, 1.0f, 1.0f, // Top-right
   1.0f, -1.0f, 1.0f, 1.0f,   1.0f, 1.0f, 1.0f, 1.0f, // Bottom-right
  -1.0f, -1.0f, 1.0f, 1.0f,   0.0f, 1.0f, 1.0f, 1.0f, // Bottom-left
  };

  glBindBuffer(GL_ARRAY_BUFFER, m_graphics_vbo);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

  glGenBuffers(1, &m_graphics_ebo);

  GLuint elements[] = {
    0, 1, 2,
    2, 3, 0,
  };

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_graphics_ebo);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(elements), elements, GL_STATIC_DRAW);

  /* Create a texute to write our image data to */
  glGenTextures(1, &m_graphics_tex);
  glBindTexture(GL_TEXTURE_2D, m_graphics_tex);

  /* Configure the texture's render settings */
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 0);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

  sr_Buffer *test = sr_newBuffer(512, 512);
  sr_floodFill(test, sr_color(255, 255, 255), 0, 0);

  for (i = 0; i < 8; i++) {
    int x = rand() % (test->w + 1);
    int y = rand() % (test->h + 1);
    sr_Pixel p = sr_getPixel(test, x, y);
    _TRACE("SAMPLE %d: %d %d %d %d\n", i, p.rgba.r, p.rgba.b, p.rgba.g, p.rgba.a)
  }

  /* Do main loop */
  double last = 0;
  // SDL_Surface *screen;
  for (;;) {
    // screen = SDL_GetVideoSurface();
    // if (screen && SDL_MUSTLOCK(screen)) SDL_LockSurface(screen);

    /* Clear OpenGL buffer */
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

    ASSERT(SDL_LockMutex(luaMutex) == 0);
    lua_getglobal(L, "sol");
    if (!lua_isnil(L, -1)) {
      lua_getfield(L, -1, "_onStep");
      if (!lua_isnil(L, -1)) {
        int err = lua_pcall(L, 0, 0, 0);
        if (err) {
          const char *str = lua_tostring(L, -1);
          fprintf(stderr, "error: %s\n", str);
          abort();
        }
      }
      lua_pop(L, 1);
    }
    ASSERT(SDL_UnlockMutex(luaMutex) == 0);
    // if (screen && SDL_MUSTLOCK(screen)) SDL_UnlockSurface(screen);

    sr_Buffer *b = test;
    // sr_Buffer *b = m_graphics_screen->buffer;
    // sr_drawRect(b, sr_color(46, 76, 24), 0, 0, 200, 200);

    /* Generate an image from Buffer data */
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, b->w, b->h, 0, GL_RGBA, GL_UNSIGNED_BYTE, b->pixels);

    /* Draw the texture to OpenGL */
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

    /* Swap the OpenGL buffers */
    SDL_GL_SwapBuffers();

    /* Wait for next frame */
    double step = (1. / m_graphics_maxFps);
    double now = SDL_GetTicks() / 1000.;
    double wait = step - (now - last);
    last += step;
    if (wait > 0) {
      SDL_Delay(wait * 1000.);
    } else {
      last = now;
    }
  }

  return 0;
}
