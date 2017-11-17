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
#include "util.h"
#include "luax.h"
#include "m_source.h"
#include "m_buffer.h"

extern double m_graphics_maxFps;
extern Buffer* m_graphics_screen;

static lua_State *L;
static SDL_mutex *luaMutex;
static GLuint vao, vbo, ebo;

static void shutdown(void) {
#ifndef __APPLE__
  SDL_UnlockMutex(luaMutex);
  SDL_Quit();
#endif
  glDeleteBuffers(1, &ebo);
  glDeleteBuffers(1, &vbo);
  glDeleteVertexArrays(1, &vao);
}

int luaopen_sol(lua_State *L);

int main(int argc, char **argv) {

  atexit(shutdown);

  // Create Vertex Array Object
  glGenVertexArrays(1, &vao);
  glBindVertexArray(vao);

  // Create a Vertex Buffer Object and copy the vertex data to it
  glGenBuffers(1, &vbo);

  float vertices[] = {
  //  Position  Color          Texcoords
  -1.0f,  1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f, // Top-left
   1.0f,  1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f, // Top-right
   1.0f, -1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, // Bottom-right
  -1.0f, -1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 1.0f, // Bottom-left
  };

  glBindBuffer(GL_ARRAY_BUFFER, vbo);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

  glGenBuffers(1, &ebo);

  GLuint elements[] = { 0, 1, 2, 2, 3, 0 };

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(elements), elements, GL_STATIC_DRAW);


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

  /* Do main loop */
  double last = 0;
  SDL_Surface *screen;
  for (;;) {
    screen = SDL_GetVideoSurface();
    if (screen && SDL_MUSTLOCK(screen)) SDL_LockSurface(screen);
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
    if (screen && SDL_MUSTLOCK(screen)) SDL_UnlockSurface(screen);

    Buffer *b = m_graphics_screen;

    /* Create and bind a 2d texture to OpenGL */
    GLuint tex;
    glGenTextures(1, &tex);
    glBindTexture(GL_TEXTURE_2D, tex);

    /* Generate an image from Buffer data */
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, b->buffer->w, b->buffer->h, 0, GL_RGBA, GL_UNSIGNED_BYTE, b->buffer->pixels);

    /* Configure the texture's render settings */
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 0);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    /* Draw the texture to OpenGL */
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

    /* Destroy the texture */
    glDeleteTextures(1, &tex);

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
